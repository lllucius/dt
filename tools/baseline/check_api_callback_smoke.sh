#!/usr/bin/env bash
# Purpose: compile and run the public DECtalk API callback smoke test against
# staged Linux install outputs.
# Scope: baseline verification using dist/include, dist/lib, and an accepted
# deterministic callback transcript.
# Behavior preservation: this script only compiles a test program, generates
# no-live-audio WAV output, records sanitized callback scalar fields, and
# compares them exactly against a committed baseline.
# Limitations: no live audio hardware, scheduler timing, queue timing, pipe
# behavior, in-memory buffers, or callback pointer payloads are exercised.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/check_api_callback_smoke.sh [--dist DIR] [--expected FILE] [--out DIR] [--cc CC]

Compiles tools/baseline/api_callback_smoke.c against the staged Linux dist tree,
runs a no-live-audio WAV callback scenario twice, checks repeatability, and
compares the callback transcript with the accepted baseline.
USAGE
}

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
dist_dir="$repo_root/dist"
expected_file="$repo_root/tests/golden/api/callback-smoke.txt"
out_dir="$repo_root/baseline-runs/api-callback-smoke"
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
if [ ! -f "$expected_file" ]; then
  echo "error: missing expected callback baseline: $expected_file" >&2
  exit 1
fi

mkdir -p "$out_dir"

binary="$out_dir/api_callback_smoke"
compile_log="$out_dir/compile.log"
run1_log="$out_dir/run-1.log"
run2_log="$out_dir/run-2.log"
actual_file="$out_dir/callback-smoke.txt"
repeat_compare="$out_dir/repeat-compare.txt"
expected_compare="$out_dir/expected-compare.txt"
wav1="$out_dir/api-callback-smoke-1.wav"
wav2="$out_dir/api-callback-smoke-2.wav"
wav1_name="api-callback-smoke-1-$$.wav"
wav2_name="api-callback-smoke-2-$$.wav"

cleanup() {
  rm -f "$dist_dir/$wav1_name" "$dist_dir/$wav2_name"
}
trap cleanup EXIT

"$cc" \
  -I"$dist_dir/include" \
  "$repo_root/tools/baseline/api_callback_smoke.c" \
  -L"$dist_dir/lib" \
  "-Wl,-rpath,$dist_dir/lib" \
  -ltts \
  -o "$binary" \
  > "$compile_log" 2>&1

(
  cd "$dist_dir"
  LD_LIBRARY_PATH="$dist_dir/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
    "$binary" "$wav1_name"
) > "$run1_log" 2>&1
mv "$dist_dir/$wav1_name" "$wav1"

(
  cd "$dist_dir"
  LD_LIBRARY_PATH="$dist_dir/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
    "$binary" "$wav2_name"
) > "$run2_log" 2>&1
mv "$dist_dir/$wav2_name" "$wav2"

if cmp -s "$run1_log" "$run2_log"; then
  printf 'api_callback_smoke_repeat=exact\n' > "$repeat_compare"
else
  {
    printf 'api_callback_smoke_repeat=different\n'
    diff -u "$run1_log" "$run2_log" || true
  } > "$repeat_compare"
  echo "api callback smoke output differs across repeated runs" >&2
  echo "repeat compare: $repeat_compare" >&2
  exit 1
fi

cp "$run1_log" "$actual_file"

if cmp -s "$expected_file" "$actual_file"; then
  printf 'api_callback_smoke_compare=exact\n' > "$expected_compare"
else
  {
    printf 'api_callback_smoke_compare=different\n'
    diff -u "$expected_file" "$actual_file" || true
  } > "$expected_compare"
  echo "api callback smoke output differs from expected baseline" >&2
  echo "expected compare: $expected_compare" >&2
  exit 1
fi

if cmp -s "$wav1" "$wav2"; then
  printf 'api_callback_smoke_wav_repeat=exact\n' >> "$repeat_compare"
else
  {
    printf 'api_callback_smoke_wav_repeat=different\n'
    sha256sum "$wav1" "$wav2"
  } >> "$repeat_compare"
  echo "api callback smoke WAV differs across repeated runs" >&2
  exit 1
fi

sha256sum "$expected_file" "$actual_file" "$wav1" "$wav2" > "$out_dir/sha256.txt"

{
  printf 'api_callback_smoke=%s\n' "$out_dir"
  printf 'binary=%s\n' "$binary"
  printf 'expected=%s\n' "$expected_file"
  printf 'actual=%s\n' "$actual_file"
  printf 'run1_log=%s\n' "$run1_log"
  printf 'run2_log=%s\n' "$run2_log"
  printf 'repeat_compare=%s\n' "$repeat_compare"
  printf 'expected_compare=%s\n' "$expected_compare"
} > "$out_dir/summary.txt"

cat "$out_dir/summary.txt"
