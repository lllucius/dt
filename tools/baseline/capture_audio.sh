#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_audio.sh [--dist DIR] [--input FILE] [--out DIR]

Generates US English WAV files for speakers 0 through 8 using dist/say.
USAGE
}

dist_dir=""
input_file=""
out_dir=""

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
    --out)
      if [ "$#" -lt 2 ]; then
        echo "error: --out requires a directory" >&2
        exit 2
      fi
      out_dir="$2"
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

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
dist_dir="${dist_dir:-$repo_root/dist}"
input_file="${input_file:-$repo_root/tests/golden/input/us_one_shot.txt}"
out_dir="${out_dir:-$repo_root/baseline-runs/audio/us}"

if [ ! -x "$dist_dir/say" ]; then
  echo "error: missing executable: $dist_dir/say" >&2
  exit 1
fi
if [ ! -d "$dist_dir/lib" ]; then
  echo "error: missing library directory: $dist_dir/lib" >&2
  exit 1
fi
if [ ! -f "$input_file" ]; then
  echo "error: missing input file: $input_file" >&2
  exit 1
fi

mkdir -p "$out_dir"
for speaker in 0 1 2 3 4 5 6 7 8; do
  LD_LIBRARY_PATH="$dist_dir/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}" \
    "$dist_dir/say" \
    -l us \
    -s "$speaker" \
    -fi "$input_file" \
    -fo "$out_dir/speaker_${speaker}.wav" \
    > "$out_dir/speaker_${speaker}.log" 2>&1
done

sha256sum "$input_file" "$out_dir"/*.wav > "$out_dir/sha256.txt"
printf '%s\n' "$out_dir"
