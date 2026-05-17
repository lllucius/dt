#!/usr/bin/env bash
# Purpose: compare expanded DECtalk WAV baseline suites against current output.
# Scope: Linux no-hardware audio regression checks for accepted suite
# directories under tests/golden/audio/us.
# Behavior preservation: exact WAV equality remains the pass/fail rule through
# compare_audio.py.
# Limitations: this script compares suite directories only; the original flat
# one-shot speaker files remain covered by compare_audio.py directly.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/compare_audio_suites.sh [--expected DIR] --actual DIR [--metrics-out DIR]

Compares accepted expanded audio suite directories. The expected directory
defaults to tests/golden/audio/us. Only child directories containing WAV files
are treated as suites.
USAGE
}

expected_dir=""
actual_dir=""
metrics_dir=""

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
expected_dir="${expected_dir:-$repo_root/tests/golden/audio/us}"
if [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi
if [[ "$actual_dir" != /* ]]; then
  actual_dir="$repo_root/$actual_dir"
fi
if [ -n "$metrics_dir" ] && [[ "$metrics_dir" != /* ]]; then
  metrics_dir="$repo_root/$metrics_dir"
fi

if [ ! -d "$expected_dir" ]; then
  echo "error: missing expected audio directory: $expected_dir" >&2
  exit 1
fi
if [ ! -d "$actual_dir" ]; then
  echo "error: missing actual audio directory: $actual_dir" >&2
  exit 1
fi

found_suite=0
for expected_suite in "$expected_dir"/*; do
  [ -d "$expected_suite" ] || continue
  if ! compgen -G "$expected_suite/*.wav" > /dev/null; then
    continue
  fi

  found_suite=1
  suite="$(basename "$expected_suite")"
  actual_suite="$actual_dir/$suite"
  echo "suite: $suite"
  if [ -n "$metrics_dir" ]; then
    mkdir -p "$metrics_dir"
    "$repo_root/tools/baseline/compare_audio.py" \
      --expected "$expected_suite" \
      --actual "$actual_suite" \
      --metrics-out "$metrics_dir/$suite.tsv"
  else
    "$repo_root/tools/baseline/compare_audio.py" \
      --expected "$expected_suite" \
      --actual "$actual_suite"
  fi
done

if [ "$found_suite" -eq 0 ]; then
  echo "error: no expected audio suites found under $expected_dir" >&2
  exit 1
fi
