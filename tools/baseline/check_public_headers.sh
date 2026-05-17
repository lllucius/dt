#!/usr/bin/env bash
# Purpose: audit DECtalk public and ABI-sensitive headers for Phase 14.
# Scope: Linux baseline verification; records installed headers and
# self-compiles the subset that is feasible without changing header contents.
# Behavior preservation: this script only reads headers and runs syntax checks;
# it does not modify public API declarations, exported symbols, or build output.
# Limitations: several installed phoneme headers intentionally contain #error
# guards or require voice-ROM selection macros, so they are inventoried instead
# of forced through isolation checks.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/check_public_headers.sh [--out DIR] [--expected DIR] [--cc CC]

Audits installed public headers and syntax-checks feasible ABI-sensitive headers.

Options:
  --out DIR       Output directory. Defaults to baseline-runs/public-headers.
  --expected DIR  Optional directory containing accepted header audit lists.
  --cc CC         C compiler to use. Defaults to CC, /usr/bin/gcc, or gcc.
USAGE
}

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
out_dir="$repo_root/baseline-runs/public-headers"
expected_dir=""
cc="${CC:-/usr/bin/gcc}"
if [ ! -x "$cc" ]; then
  cc="${CC:-gcc}"
fi

while [ "$#" -gt 0 ]; do
  case "$1" in
    --out)
      if [ "$#" -lt 2 ]; then
        echo "error: --out requires a directory" >&2
        exit 2
      fi
      out_dir="$2"
      shift 2
      ;;
    --expected)
      if [ "$#" -lt 2 ]; then
        echo "error: --expected requires a directory" >&2
        exit 2
      fi
      expected_dir="$2"
      shift 2
      ;;
    --cc)
      if [ "$#" -lt 2 ]; then
        echo "error: --cc requires a compiler path" >&2
        exit 2
      fi
      cc="$2"
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

if [[ "$out_dir" != /* ]]; then
  out_dir="$repo_root/$out_dir"
fi
if [ -n "$expected_dir" ] && [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi

mkdir -p "$out_dir/checks" "$out_dir/logs"

installed_headers=(
  src/dapi/src/api/ttsapi.h
  src/dapi/src/include/l_all_ph.h
  src/dapi/src/include/l_com_ph.h
  src/dapi/src/include/l_fr_ph.h
  src/dapi/src/include/l_gr_ph.h
  src/dapi/src/include/l_la_ph.h
  src/dapi/src/include/l_sp_ph.h
  src/dapi/src/include/l_uk_ph.h
  src/dapi/src/include/l_us_ph.h
  src/dapi/src/osf/dtmmedefs.h
)

self_compile_headers=(
  "src/dapi/src/api/tts.h tts.h"
  "src/dapi/src/api/ttsapi.h ttsapi.h"
  "src/dapi/src/include/l_com_ph.h l_com_ph.h"
  "src/dapi/src/include/l_fr_ph.h l_fr_ph.h"
  "src/dapi/src/osf/dtmmedefs.h dtmmedefs.h"
)

known_not_isolated=(
  "src/dapi/src/include/l_all_ph.h	requires explicit VOICE_ROM_* selection"
  "src/dapi/src/include/l_gr_ph.h	contains intentional #error: This file is no longer used"
  "src/dapi/src/include/l_la_ph.h	contains intentional #error: This file is no longer used"
  "src/dapi/src/include/l_sp_ph.h	contains intentional #error: This file is no longer used"
  "src/dapi/src/include/l_uk_ph.h	contains intentional #error: This file is no longer used"
  "src/dapi/src/include/l_us_ph.h	contains intentional #error: This file is no longer used"
)

write_lines() {
  local output_file="$1"
  shift
  : > "$output_file"
  for line in "$@"; do
    printf '%s\n' "$line" >> "$output_file"
  done
}

compare_expected() {
  local name="$1"

  if [ -z "$expected_dir" ]; then
    return 0
  fi

  if diff -u "$expected_dir/$name" "$out_dir/$name" > "$out_dir/$name.diff"; then
    rm -f "$out_dir/$name.diff"
    return 0
  fi

  echo "public header audit differs: $name" >&2
  echo "diff: $out_dir/$name.diff" >&2
  return 1
}

write_lines "$out_dir/installed-linux.txt" "${installed_headers[@]}"
write_lines "$out_dir/self-compile-linux.txt" "${self_compile_headers[@]}"
write_lines "$out_dir/known-not-isolated-linux.txt" "${known_not_isolated[@]}"

include_args=(
  "-I$repo_root/src/dapi/src/api"
  "-I$repo_root/src/dapi/src/osf"
  "-I$repo_root/src/dapi/src/include"
  "-I$repo_root/src/dapi/src/nt"
  "-I$repo_root/src/dapi/src/kernel"
  "-I$repo_root/src"
)

results_file="$out_dir/compile-results.tsv"
: > "$results_file"
printf 'header\tstatus\n' >> "$results_file"

for entry in "${self_compile_headers[@]}"; do
  source_path="${entry%% *}"
  include_name="${entry#* }"
  check_name="${include_name//[^A-Za-z0-9_]/_}"
  check_file="$out_dir/checks/${check_name}.c"
  log_file="$out_dir/logs/${check_name}.log"

  printf '#include "%s"\nint main(void) { return 0; }\n' "$include_name" > "$check_file"
  if "$cc" -fsyntax-only "${include_args[@]}" "$check_file" > "$log_file" 2>&1; then
    printf '%s\tok\n' "$source_path" >> "$results_file"
  else
    printf '%s\tfail\n' "$source_path" >> "$results_file"
    cat "$log_file" >&2
    exit 1
  fi
done

compare_expected installed-linux.txt
compare_expected self-compile-linux.txt
compare_expected known-not-isolated-linux.txt

printf 'public_header_audit=%s\n' "$out_dir"
