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
Usage: tools/baseline/compare_manifest.sh --expected FILE --actual FILE [--out FILE] [--mode MODE]

Compares two dist manifest files.

Modes:
  exact             Compare the complete manifest text. This is the default.
  path-type-subset  Require every expected path/type entry to exist in the
                    actual manifest, ignoring mode, size, hashes, and extra
                    actual entries.
USAGE
}

expected_file=""
actual_file=""
out_file=""
compare_mode="exact"

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
    --mode)
      if [ "$#" -lt 2 ]; then
        echo "error: --mode requires a value" >&2
        exit 2
      fi
      compare_mode="$2"
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

case "$compare_mode" in
  exact|path-type-subset)
    ;;
  *)
    echo "error: unknown compare mode: $compare_mode" >&2
    usage >&2
    exit 2
    ;;
esac

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

extract_path_types() {
  awk '
    /^[dfl] / {
      type = substr($0, 1, 1)
      rest = substr($0, 3)
      if (rest ~ /^[0-9]+ [0-9]+ /) {
        sub(/^[0-9]+ [0-9]+ /, "", rest)
      }
      split(rest, fields, " -> ")
      print type "\t" fields[1]
    }
  ' "$1" | sort -u
}

if [ "$compare_mode" = "path-type-subset" ]; then
  expected_keys="$(dirname "$out_file")/expected-path-types.tsv"
  actual_keys="$(dirname "$out_file")/actual-path-types.tsv"
  extract_path_types "$expected_file" > "$expected_keys"
  extract_path_types "$actual_file" > "$actual_keys"
  if comm -23 "$expected_keys" "$actual_keys" > "$out_file"; then
    :
  fi
  if [ ! -s "$out_file" ]; then
    extra_count="$(comm -13 "$expected_keys" "$actual_keys" | wc -l | tr -d ' ')"
    rm -f "$out_file" "$expected_keys" "$actual_keys"
    echo "manifest: ok"
    echo "mode: path-type-subset"
    echo "extra_actual_path_types: $extra_count"
    exit 0
  fi
  echo "manifest: different"
  echo "mode: path-type-subset"
  echo "missing path/type entries: $out_file"
  exit 1
fi

if diff -u "$expected_file" "$actual_file" > "$out_file"; then
  rm -f "$out_file"
  echo "manifest: ok"
else
  echo "manifest: different"
  echo "diff: $out_file"
  exit 1
fi
