#!/usr/bin/env bash
# Purpose: compare non-US DECtalk WAV baselines against current output.
# Scope: Linux no-hardware audio regression checks for accepted non-US
# language directories under tests/golden/audio.
# Behavior preservation: exact WAV equality remains the pass/fail rule through
# compare_audio.py.
# Limitations: compares one-shot per-speaker language directories only; live
# audio hardware, timing, callbacks, and queue behavior are out of scope.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/compare_non_us_audio.sh [--expected DIR] --actual DIR [--metrics-out DIR] [--language LANG]

Compares accepted non-US audio directories. The expected directory defaults to
tests/golden/audio. If --language is omitted, all accepted non-US language
directories are compared. --language may be repeated.
USAGE
}

expected_dir=""
actual_dir=""
metrics_dir=""
selected_languages=()

while [ "$#" -gt 0 ]; do
  case "$1" in
    --expected)
      if [ "$#" -lt 2 ]; then
        echo "error: --expected requires a directory" >&2
        exit 2
      fi
      expected_dir="$2"
      shift 2
      ;;
    --actual)
      if [ "$#" -lt 2 ]; then
        echo "error: --actual requires a directory" >&2
        exit 2
      fi
      actual_dir="$2"
      shift 2
      ;;
    --metrics-out)
      if [ "$#" -lt 2 ]; then
        echo "error: --metrics-out requires a directory" >&2
        exit 2
      fi
      metrics_dir="$2"
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

if [ -z "$actual_dir" ]; then
  usage >&2
  exit 2
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
expected_dir="${expected_dir:-$repo_root/tests/golden/audio}"
if [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi
if [[ "$actual_dir" != /* ]]; then
  actual_dir="$repo_root/$actual_dir"
fi
if [ -n "$metrics_dir" ] && [[ "$metrics_dir" != /* ]]; then
  metrics_dir="$repo_root/$metrics_dir"
fi

all_languages=(
  uk
  sp
  gr
  la
  fr
)

if [ "${#selected_languages[@]}" -eq 0 ]; then
  selected_languages=("${all_languages[@]}")
fi

if [ ! -d "$expected_dir" ]; then
  echo "error: missing expected audio directory: $expected_dir" >&2
  exit 1
fi
if [ ! -d "$actual_dir" ]; then
  echo "error: missing actual audio directory: $actual_dir" >&2
  exit 1
fi

for language in "${selected_languages[@]}"; do
  expected_language="$expected_dir/$language"
  actual_language="$actual_dir/$language"
  if [ ! -d "$expected_language" ]; then
    echo "error: missing expected language directory: $expected_language" >&2
    exit 1
  fi

  echo "language: $language"
  if [ -n "$metrics_dir" ]; then
    mkdir -p "$metrics_dir"
    "$repo_root/tools/baseline/compare_audio.py" \
      --expected "$expected_language" \
      --actual "$actual_language" \
      --metrics-out "$metrics_dir/$language.tsv"
  else
    "$repo_root/tools/baseline/compare_audio.py" \
      --expected "$expected_language" \
      --actual "$actual_language"
  fi
done
