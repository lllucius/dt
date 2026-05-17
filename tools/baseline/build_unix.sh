#!/usr/bin/env bash
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/build_unix.sh [--strict-warnings] [--run-dir DIR]

Runs the Unix Autotools build from src/ and captures logs.

Options:
  --strict-warnings  Clean and rebuild with a stricter warning inventory.
  --run-dir DIR      Output directory for logs. Defaults to baseline-runs/<timestamp>.
USAGE
}

strict_warnings=0
run_dir=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --strict-warnings)
      strict_warnings=1
      shift
      ;;
    --run-dir)
      if [ "$#" -lt 2 ]; then
        echo "error: --run-dir requires a directory" >&2
        exit 2
      fi
      run_dir="$2"
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
if [ -z "$run_dir" ]; then
  run_dir="$repo_root/baseline-runs/$(date -u +%Y%m%dT%H%M%SZ)"
elif [[ "$run_dir" != /* ]]; then
  run_dir="$repo_root/$run_dir"
fi

mkdir -p "$run_dir"

cc="${CC:-/usr/bin/gcc}"
if [ ! -x "$cc" ]; then
  cc="${CC:-gcc}"
fi

{
  uname -a
  "$cc" --version | sed -n '1p'
  autoreconf --version | sed -n '1p'
  automake --version | sed -n '1p'
  make --version | sed -n '1p'
  pkg-config --modversion gtk+-2.0 2>/dev/null || printf 'gtk+-2.0: not found\n'
  pkg-config --modversion alsa 2>/dev/null || printf 'alsa: not found\n'
  pkg-config --modversion libpulse 2>/dev/null || printf 'libpulse: not found\n'
} > "$run_dir/tool-versions.txt"

(cd "$repo_root/src" && autoreconf -i) > "$run_dir/autoreconf.log" 2>&1
(cd "$repo_root/src" && CC="$cc" ./configure) > "$run_dir/configure.log" 2>&1
(cd "$repo_root/src" && make clean) > "$run_dir/clean-before-build.log" 2>&1
(cd "$repo_root/src" && make) > "$run_dir/build.log" 2>&1

grep -c 'warning:' "$run_dir/build.log" > "$run_dir/default-warning-count.txt" || true
grep 'warning:' "$run_dir/build.log" \
  | sed -E 's/^.*warning: //' \
  | sed -E 's/ \[-W[^]]+\]$//' \
  | sort \
  | uniq -c \
  | sort -nr > "$run_dir/default-warning-summary.txt" || true

if [ "$strict_warnings" -eq 1 ]; then
  strict_flags='-Wall -Wextra -Wpedantic -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-declarations -Wpointer-arith -Wcast-qual -Wwrite-strings -Wbad-function-cast -Wnested-externs'
  (cd "$repo_root/src" && make clean) > "$run_dir/clean-before-strict.log" 2>&1
  (cd "$repo_root/src" && make WARN_FLAGS="$strict_flags") > "$run_dir/build-strict-warnings.log" 2>&1
  grep -c 'warning:' "$run_dir/build-strict-warnings.log" > "$run_dir/strict-warning-count.txt" || true
  grep 'warning:' "$run_dir/build-strict-warnings.log" \
    | sed -E 's/^.*warning: //' \
    | sed -E 's/ \[-W[^]]+\]$//' \
    | sort \
    | uniq -c \
    | sort -nr > "$run_dir/strict-warning-summary.txt" || true
fi

printf '%s\n' "$run_dir"
