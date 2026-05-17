#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_dist_manifest.sh [--dist DIR] [--out FILE] [--format FORMAT]

Captures a manifest of the dist tree.

Formats:
  basic          path, type, and symlink target. This is the default and keeps
                 compatibility with earlier baseline captures.
  metadata       path, type, mode, size, and symlink target.
  metadata-hash  metadata plus SHA-256 hashes for regular files.
USAGE
}

dist_dir=""
out_file=""
manifest_format="basic"

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
        echo "error: --out requires a file" >&2
        exit 2
      fi
      out_file="$2"
      shift 2
      ;;
    --format)
      if [ "$#" -lt 2 ]; then
        echo "error: --format requires a value" >&2
        exit 2
      fi
      manifest_format="$2"
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
out_file="${out_file:-$repo_root/baseline-runs/dist-manifest.txt}"

if [ ! -d "$dist_dir" ]; then
  echo "error: missing dist directory: $dist_dir" >&2
  exit 1
fi

mkdir -p "$(dirname "$out_file")"
case "$manifest_format" in
  basic)
    find "$dist_dir" -maxdepth 4 -printf '%y %P -> %l\n' | sort > "$out_file"
    ;;
  metadata)
    find "$dist_dir" -maxdepth 4 -printf '%y %m %s %P -> %l\n' | sort > "$out_file"
    ;;
  metadata-hash)
    (
      cd "$dist_dir"
      find . -maxdepth 4 -printf '%y %m %s %P -> %l\n' | sort
      find . -maxdepth 4 -type f -print0 \
        | sort -z \
        | xargs -0 sha256sum \
        | sed 's#  ./#  #'
    ) > "$out_file"
    ;;
  *)
    echo "error: unknown manifest format: $manifest_format" >&2
    usage >&2
    exit 2
    ;;
esac
printf '%s\n' "$out_file"
