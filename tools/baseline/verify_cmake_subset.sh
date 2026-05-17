#!/usr/bin/env bash
# Purpose: verify the side-by-side CMake Linux subset without promoting it.
# Scope: CMake configure/build/stage checks plus exact dictionary/audio checks
# and exported-symbol checks that are valid for the current CMake subset.
# Behavior preservation: this script reads and compares generated artifacts; it
# does not replace Autotools, change install layout, or update accepted outputs.
# Limitations: CMake packaging remains a subset, so staged-layout differences
# are captured but not treated as parity in this script.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/verify_cmake_subset.sh [--run-dir DIR] [--expected DIR] [--symbol-mode MODE]

Builds the CMake subset and verifies dictionaries, US WAV output,
compile_commands.json, libtts.so symbols, and language-library symbol names.

Modes:
  exact      Compare CMake-staged libtts.so against the exact committed symbol
             capture. This is the default for local reproducibility gates.
  name-type  Compare CMake-staged libtts.so symbol type/name sets only. This is
             useful on hosted CI runners where symbol addresses may vary.
USAGE
}

run_dir=""
expected_dir=""
symbol_mode="exact"

while [ "$#" -gt 0 ]; do
  case "$1" in
    --run-dir)
      if [ "$#" -lt 2 ]; then
        echo "error: --run-dir requires a directory" >&2
        exit 2
      fi
      run_dir="$2"
      shift 2
      ;;
    --expected)
      if [ "$#" -lt 2 ]; then
        echo "error: --expected requires a directory" >&2
        exit 2
      fi
      expected_dir="$2"
      shift 2
      ;;
    --symbol-mode)
      if [ "$#" -lt 2 ]; then
        echo "error: --symbol-mode requires a value" >&2
        exit 2
      fi
      symbol_mode="$2"
      shift 2
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "error: unknown option: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
done

case "$symbol_mode" in
  exact|name-type)
    ;;
  *)
    echo "error: unknown symbol mode: $symbol_mode" >&2
    usage >&2
    exit 2
    ;;
esac

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if [ -z "$run_dir" ]; then
  run_dir="$repo_root/baseline-runs/cmake-subset"
elif [[ "$run_dir" != /* ]]; then
  run_dir="$repo_root/$run_dir"
fi
if [ -n "$expected_dir" ] && [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi

mkdir -p "$run_dir"
build_dir="$run_dir/build"
dist_dir="$build_dir/cmake-dist"
cc="${CC:-/usr/bin/gcc}"
if [ ! -x "$cc" ]; then
  cc="${CC:-gcc}"
fi

CC="$cc" cmake -S "$repo_root" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release \
  > "$run_dir/cmake-configure.log" 2>&1
cmake --build "$build_dir" --target dectalk_cmake_stage -- -j1 \
  > "$run_dir/cmake-build.log" 2>&1

if [ ! -s "$build_dir/compile_commands.json" ]; then
  echo "error: missing compile_commands.json" >&2
  exit 1
fi

"$repo_root/tools/baseline/capture_dist_manifest.sh" \
  --dist "$dist_dir" \
  --format metadata-hash \
  --out "$run_dir/dist-manifest-detailed.txt" > "$run_dir/dist-manifest-detailed.log"

"$repo_root/tools/baseline/capture_dictionaries.sh" \
  --dist "$dist_dir" \
  --out "$run_dir/dictionaries"
if [ -n "$expected_dir" ] && [ -d "$expected_dir/dictionaries" ]; then
  "$repo_root/tools/baseline/compare_dictionaries.sh" \
    --expected "$expected_dir/dictionaries" \
    --actual "$run_dir/dictionaries" \
    --out "$run_dir/dictionary-compare" > "$run_dir/dictionary-compare.txt"
fi

"$repo_root/tools/baseline/capture_audio.sh" \
  --dist "$dist_dir" \
  --out "$run_dir/audio-us"
"$repo_root/tools/baseline/compare_audio.py" \
  --actual "$run_dir/audio-us" \
  --metrics-out "$run_dir/audio-metrics.tsv" > "$run_dir/audio-compare.txt"
"$repo_root/tools/baseline/capture_audio_suites.sh" \
  --dist "$dist_dir" \
  --out "$run_dir/audio-us-suites"
"$repo_root/tools/baseline/compare_audio_suites.sh" \
  --actual "$run_dir/audio-us-suites" \
  --metrics-out "$run_dir/audio-suite-metrics" > "$run_dir/audio-suite-compare.txt"

"$repo_root/tools/baseline/capture_symbols.sh" \
  --dist "$dist_dir" \
  --out "$run_dir/symbols"
if [ -n "$expected_dir" ] && [ -d "$expected_dir/symbols" ]; then
  if [ "$symbol_mode" = "name-type" ]; then
    awk '{ print $2 "\t" $3 }' "$expected_dir/symbols/symbols-libtts.so.txt" | sort \
      > "$run_dir/expected-names-symbols-libtts.so.txt"
    awk '{ print $2 "\t" $3 }' "$run_dir/symbols/symbols-libtts.so.txt" | sort \
      > "$run_dir/actual-names-symbols-libtts.so.txt"
    diff -u \
      "$run_dir/expected-names-symbols-libtts.so.txt" \
      "$run_dir/actual-names-symbols-libtts.so.txt" \
      > "$run_dir/libtts-symbol-compare.diff"
  else
    diff -u \
      "$expected_dir/symbols/symbols-libtts.so.txt" \
      "$run_dir/symbols/symbols-libtts.so.txt" \
      > "$run_dir/libtts-symbol-compare.diff"
  fi
  rm -f "$run_dir/libtts-symbol-compare.diff"

  : > "$run_dir/language-symbol-name-compare.txt"
  for expected_symbols in "$expected_dir"/symbols/symbols-libtts_*.so.txt; do
    base="$(basename "$expected_symbols")"
    actual_symbols="$run_dir/symbols/$base"
    expected_names="$run_dir/expected-names-$base"
    actual_names="$run_dir/actual-names-$base"
    awk '{ print $2 "\t" $3 }' "$expected_symbols" | sort > "$expected_names"
    awk '{ print $2 "\t" $3 }' "$actual_symbols" | sort > "$actual_names"
    diff -u "$expected_names" "$actual_names" > "$run_dir/$base.names.diff"
    rm -f "$run_dir/$base.names.diff"
    printf '%s\tok\n' "$base" >> "$run_dir/language-symbol-name-compare.txt"
  done
fi

{
  printf 'run_dir=%s\n' "$run_dir"
  printf 'build_dir=%s\n' "$build_dir"
  printf 'dist_dir=%s\n' "$dist_dir"
  printf 'compile_commands=%s\n' "$build_dir/compile_commands.json"
  [ -f "$run_dir/dictionary-compare.txt" ] && printf 'dictionary_compare=%s\n' "$run_dir/dictionary-compare.txt"
  printf 'audio_compare=%s\n' "$run_dir/audio-compare.txt"
  printf 'audio_suite_compare=%s\n' "$run_dir/audio-suite-compare.txt"
  [ -f "$run_dir/language-symbol-name-compare.txt" ] && printf 'language_symbol_names=%s\n' "$run_dir/language-symbol-name-compare.txt"
  printf 'dist_manifest_detailed=%s\n' "$run_dir/dist-manifest-detailed.txt"
} > "$run_dir/summary.txt"

cat "$run_dir/summary.txt"
