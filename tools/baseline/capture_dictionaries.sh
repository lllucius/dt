#!/usr/bin/env bash
# Purpose: capture hashes for generated DECtalk dictionary artifacts.
# Scope: baseline verification of dist/dic outputs only.
# Behavior preservation: dictionary hash changes are treated as behavior-relevant
# until explicitly reviewed.
# Limitations: this script hashes generated files; it does not build them.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_dictionaries.sh [--dist DIR] [--out DIR]

Captures SHA-256 hashes and file metadata for generated dist/dic/*.dic files.
USAGE
}

dist_dir=""
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
out_dir="${out_dir:-$repo_root/baseline-runs/dictionaries}"
if [[ "$dist_dir" != /* ]]; then
  dist_dir="$repo_root/$dist_dir"
fi
if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

dic_dir="$dist_dir/dic"
if [ ! -d "$dic_dir" ]; then
  echo "error: missing dictionary directory: $dic_dir" >&2
  exit 1
fi

mkdir -p "$out_dir"
find "$dic_dir" -maxdepth 1 -type f -name '*.dic' -printf '%f\n' | sort > "$out_dir/files.txt"
if [ ! -s "$out_dir/files.txt" ]; then
  echo "error: no dictionary files found in $dic_dir" >&2
  exit 1
fi

while IFS= read -r file; do
  sha256sum "$dic_dir/$file"
done < "$out_dir/files.txt" | sed "s#  $dic_dir/#  #" > "$out_dir/sha256.txt"

while IFS= read -r file; do
  stat -c '%n	%s' "$dic_dir/$file" | sed "s#^$dic_dir/##"
done < "$out_dir/files.txt" > "$out_dir/sizes.tsv"

printf '%s\n' "$out_dir"
