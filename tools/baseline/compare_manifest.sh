#!/usr/bin/env bash
# Purpose: compare captured DECtalk dist manifests.
# Scope: baseline verification only; this script compares text manifests and
# writes a diff report.
# Behavior preservation: install-layout changes become explicit before packaging
# cleanup proceeds.
# Limitations: manifest detail depends on the capture mode used by the caller.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/compare_manifest.sh --expected FILE --actual FILE [--out FILE]

Compares two dist manifest files.
USAGE
}

expected_file=""
actual_file=""
out_file=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --expected)
      if [ "$#" -lt 2 ]; then
        echo "error: --expected requires a file" >&2
        exit 2
      fi
      expected_file="$2"
      shift 2
      ;;
    --actual)
      if [ "$#" -lt 2 ]; then
        echo "error: --actual requires a file" >&2
        exit 2
      fi
      actual_file="$2"
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

if [ -z "$expected_file" ] || [ -z "$actual_file" ]; then
  usage >&2
  exit 2
fi

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
if [[ "$expected_file" != /* ]]; then
  expected_file="$repo_root/$expected_file"
fi
if [[ "$actual_file" != /* ]]; then
  actual_file="$repo_root/$actual_file"
fi
if [ -z "$out_file" ]; then
  out_file="$repo_root/baseline-runs/manifest.diff"
elif [[ "$out_file" != /* ]]; then
  out_file="$repo_root/$out_file"
fi

mkdir -p "$(dirname "$out_file")"
if diff -u "$expected_file" "$actual_file" > "$out_file"; then
  rm -f "$out_file"
  echo "manifest: ok"
else
  echo "manifest: different"
  echo "diff: $out_file"
  exit 1
fi
