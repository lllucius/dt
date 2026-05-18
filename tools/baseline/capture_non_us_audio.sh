#!/usr/bin/env bash
# Purpose: generate deterministic non-US DECtalk WAV baseline outputs.
# Scope: Linux no-hardware verification using dist/say, committed one-shot
# language inputs, and the existing per-speaker capture script.
# Behavior preservation: this script only generates WAV files for later exact
# comparison; it does not update accepted baselines unless the caller writes
# into tests/golden intentionally.
# Limitations: covers file-output WAV generation only, not live audio hardware,
# callback timing, queue behavior, or every language-specific parser feature.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_non_us_audio.sh [--dist DIR] [--out DIR] [--language LANG]

Generates non-US one-shot WAV files for speakers 0 through 8. If --language is
omitted, all accepted non-US language candidates are generated. --language may
be repeated.

Languages:
  uk
  sp
  gr
  la
  fr
USAGE
}

dist_dir=""
out_dir=""
selected_languages=()

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
    --language)
      if [ "$#" -lt 2 ]; then
        echo "error: --language requires a language code" >&2
        exit 2
      fi
      selected_languages+=("$2")
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
out_dir="${out_dir:-$repo_root/baseline-runs/audio/non-us}"
if [[ "$dist_dir" != /* ]]; then
  dist_dir="$repo_root/$dist_dir"
fi
if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

all_languages=(
  uk
  sp
  gr
  la
  fr
)

language_input() {
  case "$1" in
    uk)
      printf '%s\n' "$repo_root/tests/golden/input/uk_one_shot.txt"
      ;;
    sp)
      printf '%s\n' "$repo_root/tests/golden/input/sp_one_shot.txt"
      ;;
    gr)
      printf '%s\n' "$repo_root/tests/golden/input/gr_one_shot.txt"
      ;;
    la)
      printf '%s\n' "$repo_root/tests/golden/input/la_one_shot.txt"
      ;;
    fr)
      printf '%s\n' "$repo_root/tests/golden/input/fr_one_shot.txt"
      ;;
    *)
      echo "error: unknown non-US language: $1" >&2
      usage >&2
      exit 2
      ;;
  esac
}

if [ "${#selected_languages[@]}" -eq 0 ]; then
  selected_languages=("${all_languages[@]}")
fi

mkdir -p "$out_dir"
for language in "${selected_languages[@]}"; do
  input_file="$(language_input "$language")"
  "$repo_root/tools/baseline/capture_audio.sh" \
    --dist "$dist_dir" \
    --language "$language" \
    --input "$input_file" \
    --out "$out_dir/$language" > "$out_dir/$language.capture.txt"
done

printf '%s\n' "$out_dir"
