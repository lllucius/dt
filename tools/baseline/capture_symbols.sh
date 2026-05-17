#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_symbols.sh [--dist DIR] [--out DIR]

Captures defined dynamic symbols for every shared library in dist/lib.
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
out_dir="${out_dir:-$repo_root/baseline-runs/symbols}"

mkdir -p "$out_dir"

if [ ! -d "$dist_dir/lib" ]; then
  echo "error: missing library directory: $dist_dir/lib" >&2
  exit 1
fi

found=0
for lib in "$dist_dir"/lib/*.so "$dist_dir"/lib/*.dylib; do
  [ -e "$lib" ] || continue
  found=1
  base="$(basename "$lib")"
  nm -D --defined-only "$lib" | sort > "$out_dir/symbols-${base}.txt"
done

if [ "$found" -eq 0 ]; then
  echo "error: no shared libraries found in $dist_dir/lib" >&2
  exit 1
fi

printf '%s\n' "$out_dir"
