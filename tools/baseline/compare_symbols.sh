#!/usr/bin/env bash
# Purpose: compare captured DECtalk dynamic symbol lists.
# Scope: baseline verification only; this script does not build or modify
# DECtalk outputs.
# Behavior preservation: exact symbol-list diffs are reported so API/ABI drift is
# visible before source cleanup proceeds.
# Limitations: comparison quality depends on a trusted expected symbol capture.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/compare_symbols.sh --expected DIR --actual DIR [--out DIR]

Compares symbols-*.txt files produced by capture_symbols.sh.
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
  out_dir="$repo_root/baseline-runs/symbol-compare"
elif [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

if [ ! -d "$expected_dir" ]; then
  echo "error: missing expected directory: $expected_dir" >&2
  exit 1
fi
if [ ! -d "$actual_dir" ]; then
  echo "error: missing actual directory: $actual_dir" >&2
  exit 1
fi

mkdir -p "$out_dir"
summary="$out_dir/summary.tsv"
printf 'file\tstatus\n' > "$summary"

failed=0
for expected in "$expected_dir"/symbols-*.txt; do
  [ -e "$expected" ] || continue
  base="$(basename "$expected")"
  actual="$actual_dir/$base"
  if [ ! -f "$actual" ]; then
    printf '%s\tmissing-actual\n' "$base" >> "$summary"
    failed=1
    continue
  fi
  if diff -u "$expected" "$actual" > "$out_dir/diff-$base"; then
    printf '%s\tok\n' "$base" >> "$summary"
    rm -f "$out_dir/diff-$base"
  else
    printf '%s\tdifferent\n' "$base" >> "$summary"
    failed=1
  fi
done

for actual in "$actual_dir"/symbols-*.txt; do
  [ -e "$actual" ] || continue
  base="$(basename "$actual")"
  if [ ! -f "$expected_dir/$base" ]; then
    printf '%s\tunexpected-actual\n' "$base" >> "$summary"
    failed=1
  fi
done

if [ "$(wc -l < "$summary")" -eq 1 ]; then
  echo "error: no symbol files found to compare" >&2
  exit 1
fi

cat "$summary"
exit "$failed"
