#!/usr/bin/env bash
# Purpose: compile and run the public DECtalk API smoke test against staged
# Linux install outputs.
# Scope: baseline verification using dist/include, dist/lib, and committed
# deterministic US English input.
# Behavior preservation: this script only compiles a test program, checks
# stable public API scalar behavior, generates a WAV through public APIs, and
# compares it against an accepted golden WAV.
# Limitations: no live audio hardware, callback, in-memory output,
# phoneme-capture path, timing-sensitive metadata, or non-US speech output is
# exercised.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/check_api_smoke.sh [--dist DIR] [--input FILE] [--expected FILE] [--out DIR] [--cc CC]

Compiles tools/baseline/api_smoke.c against the staged Linux dist tree, checks
stable public API scalar behavior, runs startup/speak-to-WAV/close/shutdown
through the public API, and compares the generated WAV with an accepted
deterministic baseline.
USAGE
}

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
dist_dir="$repo_root/dist"
input_file="$repo_root/tests/golden/input/us_one_shot.txt"
expected_file="$repo_root/tests/golden/audio/us/speaker_0.wav"
out_dir="$repo_root/baseline-runs/api-smoke"
cc="${CC:-/usr/bin/gcc}"
if [ ! -x "$cc" ]; then
  cc="${CC:-gcc}"
fi

while [ "$#" -gt 0 ]; do
  case "$1" in
    --dist)
      if [ "$#" -lt 2 ]; then
        echo "error: --dist requires a directory" >&2
        exit 2
      fi
      dist_dir="$2"
      shift 2
      ;;
    --input)
      if [ "$#" -lt 2 ]; then
        echo "error: --input requires a file" >&2
        exit 2
      fi
      input_file="$2"
      shift 2
      ;;
    --expected)
      if [ "$#" -lt 2 ]; then
        echo "error: --expected requires a file" >&2
        exit 2
      fi
      expected_file="$2"
      shift 2
      ;;
    --out)
      if [ "$#" -lt 2 ]; then
        echo "error: --out requires a directory" >&2
        exit 2
      fi
      out_dir="$2"
      shift 2
      ;;
    --cc)
      if [ "$#" -lt 2 ]; then
        echo "error: --cc requires a compiler path" >&2
        exit 2
      fi
      cc="$2"
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

if [[ "$dist_dir" != /* ]]; then
  dist_dir="$repo_root/$dist_dir"
fi
if [[ "$input_file" != /* ]]; then
  input_file="$repo_root/$input_file"
fi
if [[ "$expected_file" != /* ]]; then
  expected_file="$repo_root/$expected_file"
fi
if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

if [ ! -f "$dist_dir/include/dtk/ttsapi.h" ]; then
  echo "error: missing installed public header: $dist_dir/include/dtk/ttsapi.h" >&2
  exit 1
fi
if [ ! -f "$dist_dir/lib/libtts.so" ]; then
  echo "error: missing staged library: $dist_dir/lib/libtts.so" >&2
  exit 1
fi
if [ ! -f "$input_file" ]; then
  echo "error: missing input file: $input_file" >&2
  exit 1
fi
if [ ! -f "$expected_file" ]; then
  echo "error: missing expected WAV: $expected_file" >&2
  exit 1
fi

mkdir -p "$out_dir"

binary="$out_dir/api_smoke"
actual_file="$out_dir/api-smoke-us-speaker-0.wav"
compile_log="$out_dir/compile.log"
run_log="$out_dir/run.log"
compare_log="$out_dir/compare.log"

"$cc" \
  -I"$dist_dir/include" \
  "$repo_root/tools/baseline/api_smoke.c" \
  -L"$dist_dir/lib" \
  "-Wl,-rpath,$dist_dir/lib" \
  -ltts \
  -o "$binary" \
  > "$compile_log" 2>&1

(
  cd "$dist_dir"
  LD_LIBRARY_PATH="$dist_dir/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
    "$binary" "$input_file" "$actual_file"
) > "$run_log" 2>&1

if cmp -s "$expected_file" "$actual_file"; then
  printf 'api_smoke_compare=exact\n' > "$compare_log"
else
  {
    printf 'api_smoke_compare=different\n'
    sha256sum "$expected_file" "$actual_file"
  } > "$compare_log"
  echo "api smoke WAV differs from expected baseline" >&2
  echo "compare log: $compare_log" >&2
  exit 1
fi

sha256sum "$input_file" "$expected_file" "$actual_file" > "$out_dir/sha256.txt"

{
  printf 'api_smoke=%s\n' "$out_dir"
  printf 'binary=%s\n' "$binary"
  printf 'input=%s\n' "$input_file"
  printf 'expected=%s\n' "$expected_file"
  printf 'actual=%s\n' "$actual_file"
  printf 'run_log=%s\n' "$run_log"
  printf 'compare=%s\n' "$compare_log"
} > "$out_dir/summary.txt"

cat "$out_dir/summary.txt"
