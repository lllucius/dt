#!/usr/bin/env bash
# Purpose: compare generated DECtalk dictionary hash captures.
# Scope: baseline verification only; this script does not inspect dictionary
# internals.
# Behavior preservation: exact generated dictionary changes are surfaced before
# dictionary-related modernization proceeds.
# Limitations: hash equality proves byte equality, not semantic correctness.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/compare_dictionaries.sh --expected DIR --actual DIR [--out DIR]

Compares outputs produced by capture_dictionaries.sh.
USAGE
}

expected_dir=""
actual_dir=""
out_dir=""

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

if [ -z "$expected_dir" ] || [ -z "$actual_dir" ]; then
  usage >&2
  exit 2
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi
if [[ "$actual_dir" != /* ]]; then
  actual_dir="$repo_root/$actual_dir"
fi
if [ -z "$out_dir" ]; then
  out_dir="$repo_root/baseline-runs/dictionary-compare"
elif [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

mkdir -p "$out_dir"
failed=0
for file in files.txt sha256.txt sizes.tsv; do
  if diff -u "$expected_dir/$file" "$actual_dir/$file" > "$out_dir/diff-$file"; then
    rm -f "$out_dir/diff-$file"
    printf '%s\tok\n' "$file"
  else
    printf '%s\tdifferent\n' "$file"
    failed=1
  fi
done

exit "$failed"
