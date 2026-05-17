#!/usr/bin/env bash
# Purpose: compile deterministic user dictionary fixtures and capture hashes.
# Scope: baseline verification for committed tests/golden/dictionaries/user
# fixture inputs.
# Behavior preservation: user dictionary compiler output changes are treated as
# behavior-relevant until explicitly reviewed.
# Limitations: this script records byte-level output equality; it does not
# inspect dictionary semantics.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/capture_user_dictionaries.sh [--fixtures DIR] [--compiler FILE] [--out DIR]

Compiles user dictionary fixture .tab files and captures files.txt, sha256.txt,
and sizes.tsv for the generated .dtu outputs.
USAGE
}

fixture_dir=""
compiler=""
out_dir=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --fixtures)
      if [ "$#" -lt 2 ]; then
        echo "error: --fixtures requires a directory" >&2
        exit 2
      fi
      fixture_dir="$2"
      shift 2
      ;;
    --compiler)
      if [ "$#" -lt 2 ]; then
        echo "error: --compiler requires a file" >&2
        exit 2
      fi
      compiler="$2"
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
fixture_dir="${fixture_dir:-$repo_root/tests/golden/dictionaries/user/input}"
compiler="${compiler:-$repo_root/dist/tools/udic_us}"
out_dir="${out_dir:-$repo_root/baseline-runs/user-dictionaries}"

if [[ "$fixture_dir" != /* ]]; then
  fixture_dir="$repo_root/$fixture_dir"
fi
if [[ "$compiler" != /* ]]; then
  compiler="$repo_root/$compiler"
fi
if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi

if [ ! -d "$fixture_dir" ]; then
  echo "error: missing fixture directory: $fixture_dir" >&2
  exit 1
fi
if [ ! -x "$compiler" ]; then
  echo "error: missing executable compiler: $compiler" >&2
  exit 1
fi

dic_dir="$out_dir/dic"
log_dir="$out_dir/logs"
mkdir -p "$dic_dir" "$log_dir"

shopt -s nullglob
fixtures=("$fixture_dir"/*.tab)
if [ "${#fixtures[@]}" -eq 0 ]; then
  echo "error: no .tab fixtures found in $fixture_dir" >&2
  exit 1
fi

LC_ALL=C
for fixture in "${fixtures[@]}"; do
  base="$(basename "$fixture" .tab)"
  "$compiler" "$fixture" "$dic_dir/$base.dtu" > "$log_dir/$base.log" 2>&1
done

find "$dic_dir" -maxdepth 1 -type f -name '*.dtu' -printf '%f\n' | sort > "$out_dir/files.txt"
if [ ! -s "$out_dir/files.txt" ]; then
  echo "error: no user dictionary outputs found in $dic_dir" >&2
  exit 1
fi

while IFS= read -r file; do
  sha256sum "$dic_dir/$file"
done < "$out_dir/files.txt" | sed "s#  $dic_dir/#  #" > "$out_dir/sha256.txt"

while IFS= read -r file; do
  stat -c '%n	%s' "$dic_dir/$file" | sed "s#^$dic_dir/##"
done < "$out_dir/files.txt" > "$out_dir/sizes.tsv"

printf '%s\n' "$out_dir"
