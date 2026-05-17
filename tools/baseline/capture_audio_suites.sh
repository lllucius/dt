#!/usr/bin/env bash
# Purpose: generate deterministic DECtalk WAV outputs for accepted US audio
# baseline suites beyond the original one-shot input.
# Scope: Linux no-hardware verification using dist/say, committed text inputs,
# and the existing per-speaker capture script.
# Behavior preservation: this script only generates files for later exact
# comparison; it does not update accepted baselines unless the caller writes
# into tests/golden intentionally.
# Limitations: current suites are US English only and do not exercise live audio
# devices, callback timing, or non-US language dictionaries.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_audio_suites.sh [--dist DIR] [--out DIR] [--suite NAME]

Generates accepted expanded US English WAV suites. If --suite is omitted, all
accepted suites are generated. --suite may be repeated.

Suites:
  us_abbreviations
  us_commands_markup
  us_punctuation_numbers
USAGE
}

dist_dir=""
out_dir=""
selected_suites=()

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
    --out)
      if [ "$#" -lt 2 ]; then
        echo "error: --out requires a directory" >&2
        exit 2
      fi
      out_dir="$2"
      shift 2
      ;;
    --suite)
      if [ "$#" -lt 2 ]; then
        echo "error: --suite requires a suite name" >&2
        exit 2
      fi
      selected_suites+=("$2")
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
out_dir="${out_dir:-$repo_root/baseline-runs/audio/us-suites}"
if [[ "$dist_dir" != /* ]]; then
  dist_dir="$repo_root/$dist_dir"
fi
if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

all_suites=(
  us_abbreviations
  us_commands_markup
  us_punctuation_numbers
)

suite_input() {
  case "$1" in
    us_abbreviations)
      printf '%s\n' "$repo_root/tests/golden/input/us_abbreviations.txt"
      ;;
    us_commands_markup)
      printf '%s\n' "$repo_root/tests/golden/input/us_commands_markup.txt"
      ;;
    us_punctuation_numbers)
      printf '%s\n' "$repo_root/tests/golden/input/us_punctuation_numbers.txt"
      ;;
    *)
      echo "error: unknown audio suite: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
}

if [ "${#selected_suites[@]}" -eq 0 ]; then
  selected_suites=("${all_suites[@]}")
fi

mkdir -p "$out_dir"
for suite in "${selected_suites[@]}"; do
  input_file="$(suite_input "$suite")"
  "$repo_root/tools/baseline/capture_audio.sh" \
    --dist "$dist_dir" \
    --input "$input_file" \
    --out "$out_dir/$suite" > "$out_dir/$suite.capture.txt"
done

printf '%s\n' "$out_dir"
