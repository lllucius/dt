#!/usr/bin/env bash
# Purpose: run the current DECtalk baseline verification sequence.
# Scope: local Linux verification using the existing Autotools build and
# baseline capture scripts.
# Behavior preservation: this script centralizes exact audio, symbol, manifest,
# and dictionary checks so regressions are visible before modernization proceeds.
# Limitations: symbol, manifest, and dictionary comparisons require an expected
# run directory supplied by the caller.
set -euo pipefail

usage() {
  cat <<'USAGE'
Usage: tools/baseline/verify_current.sh [--run-dir DIR] [--expected DIR]

Runs build, capture, and comparison steps. The expected directory is optional.
When supplied, it may contain:

  symbols/
  dist-manifest.txt
  dist-manifest-detailed.txt
  dictionaries/
  dictionaries/user/expected/
  public-headers/
  warnings/default-cleaned.tsv

Golden audio is always compared against tests/golden/audio/us, including
expanded suite directories when present.
USAGE
}

run_dir=""
expected_dir=""

while [ "$#" -gt 0 ]; do
  case "$1" in
    --run-dir)
      if [ "$#" -lt 2 ]; then
        echo "error: --run-dir requires a directory" >&2
        exit 2
      fi
      run_dir="$2"
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
  run_dir="$repo_root/baseline-runs/verify-$(date -u +%Y%m%dT%H%M%SZ)"
elif [[ "$run_dir" != /* ]]; then
  run_dir="$repo_root/$run_dir"
fi
if [ -n "$expected_dir" ] && [[ "$expected_dir" != /* ]]; then
  expected_dir="$repo_root/$expected_dir"
fi

mkdir -p "$run_dir"

"$repo_root/tools/baseline/build_unix.sh" --strict-warnings --run-dir "$run_dir/build"
"$repo_root/tools/baseline/summarize_warnings.py" \
  --log "$run_dir/build/build.log" \
  --out-dir "$run_dir/warnings-default"
"$repo_root/tools/baseline/summarize_warnings.py" \
  --log "$run_dir/build/build-strict-warnings.log" \
  --out-dir "$run_dir/warnings-strict"
"$repo_root/tools/baseline/capture_symbols.sh" --out "$run_dir/symbols"
"$repo_root/tools/baseline/capture_dist_manifest.sh" --out "$run_dir/dist-manifest.txt"
"$repo_root/tools/baseline/capture_dist_manifest.sh" \
  --format metadata-hash \
  --out "$run_dir/dist-manifest-detailed.txt"
"$repo_root/tools/baseline/capture_dictionaries.sh" --out "$run_dir/dictionaries"
"$repo_root/tools/baseline/capture_user_dictionaries.sh" --out "$run_dir/user-dictionaries"
"$repo_root/tools/baseline/check_public_headers.sh" --out "$run_dir/public-headers"
"$repo_root/tools/baseline/check_api_smoke.sh" --out "$run_dir/api-smoke"
"$repo_root/tools/baseline/check_api_callback_smoke.sh" --out "$run_dir/api-callback-smoke"
"$repo_root/tools/baseline/capture_audio.sh" --out "$run_dir/audio-us"
"$repo_root/tools/baseline/compare_audio.py" \
  --actual "$run_dir/audio-us" \
  --metrics-out "$run_dir/audio-metrics.tsv" > "$run_dir/audio-compare.txt"
"$repo_root/tools/baseline/capture_audio_suites.sh" --out "$run_dir/audio-us-suites"
"$repo_root/tools/baseline/compare_audio_suites.sh" \
  --actual "$run_dir/audio-us-suites" \
  --metrics-out "$run_dir/audio-suite-metrics" > "$run_dir/audio-suite-compare.txt"
"$repo_root/tools/baseline/capture_non_us_audio.sh" --out "$run_dir/audio-non-us"
"$repo_root/tools/baseline/compare_non_us_audio.sh" \
  --actual "$run_dir/audio-non-us" \
  --metrics-out "$run_dir/audio-non-us-metrics" > "$run_dir/audio-non-us-compare.txt"

if [ -n "$expected_dir" ]; then
  if [ -d "$expected_dir/symbols" ]; then
    "$repo_root/tools/baseline/compare_symbols.sh" \
      --expected "$expected_dir/symbols" \
      --actual "$run_dir/symbols" \
      --out "$run_dir/symbol-compare" > "$run_dir/symbol-compare.txt"
  fi
  if [ -f "$expected_dir/dist-manifest.txt" ]; then
    "$repo_root/tools/baseline/compare_manifest.sh" \
      --expected "$expected_dir/dist-manifest.txt" \
      --actual "$run_dir/dist-manifest.txt" \
      --out "$run_dir/manifest.diff" > "$run_dir/manifest-compare.txt"
  fi
  if [ -f "$expected_dir/dist-manifest-detailed.txt" ]; then
    "$repo_root/tools/baseline/compare_manifest.sh" \
      --expected "$expected_dir/dist-manifest-detailed.txt" \
      --actual "$run_dir/dist-manifest-detailed.txt" \
      --out "$run_dir/manifest-detailed.diff" > "$run_dir/manifest-detailed-compare.txt"
  fi
  if [ -d "$expected_dir/dictionaries" ]; then
    "$repo_root/tools/baseline/compare_dictionaries.sh" \
      --expected "$expected_dir/dictionaries" \
      --actual "$run_dir/dictionaries" \
      --out "$run_dir/dictionary-compare" > "$run_dir/dictionary-compare.txt"
  fi
  if [ -d "$expected_dir/dictionaries/user/expected" ]; then
    "$repo_root/tools/baseline/compare_dictionaries.sh" \
      --expected "$expected_dir/dictionaries/user/expected" \
      --actual "$run_dir/user-dictionaries" \
      --out "$run_dir/user-dictionary-compare" > "$run_dir/user-dictionary-compare.txt"
  fi
  if [ -d "$expected_dir/public-headers" ]; then
    "$repo_root/tools/baseline/check_public_headers.sh" \
      --out "$run_dir/public-headers" \
      --expected "$expected_dir/public-headers" > "$run_dir/public-header-compare.txt"
  fi
  if [ -f "$expected_dir/warnings/default-cleaned.tsv" ]; then
    "$repo_root/tools/baseline/check_warning_budgets.py" \
      --warnings "$run_dir/warnings-default/warnings.tsv" \
      --budget "$expected_dir/warnings/default-cleaned.tsv" \
      --out "$run_dir/warning-budget.tsv" > "$run_dir/warning-budget.txt"
  fi
  if [ -f "$expected_dir/warnings/strict-cleaned.tsv" ]; then
    "$repo_root/tools/baseline/check_warning_budgets.py" \
      --warnings "$run_dir/warnings-strict/warnings.tsv" \
      --budget "$expected_dir/warnings/strict-cleaned.tsv" \
      --out "$run_dir/warning-budget-strict.tsv" > "$run_dir/warning-budget-strict.txt"
  fi
fi

{
  printf 'run_dir=%s\n' "$run_dir"
  printf 'default_warnings=%s\n' "$(cat "$run_dir/build/default-warning-count.txt")"
  printf 'strict_warnings=%s\n' "$(cat "$run_dir/build/strict-warning-count.txt")"
  printf 'parser_default_warnings=%s\n' "$(($(wc -l < "$run_dir/warnings-default/warnings.tsv") - 1))"
  printf 'parser_strict_warnings=%s\n' "$(($(wc -l < "$run_dir/warnings-strict/warnings.tsv") - 1))"
  printf 'audio_compare=%s\n' "$run_dir/audio-compare.txt"
  printf 'audio_metrics=%s\n' "$run_dir/audio-metrics.tsv"
  printf 'audio_suite_compare=%s\n' "$run_dir/audio-suite-compare.txt"
  printf 'audio_suite_metrics=%s\n' "$run_dir/audio-suite-metrics"
  printf 'audio_non_us_compare=%s\n' "$run_dir/audio-non-us-compare.txt"
  printf 'audio_non_us_metrics=%s\n' "$run_dir/audio-non-us-metrics"
  [ -f "$run_dir/symbol-compare.txt" ] && printf 'symbol_compare=%s\n' "$run_dir/symbol-compare.txt"
  [ -f "$run_dir/manifest-compare.txt" ] && printf 'manifest_compare=%s\n' "$run_dir/manifest-compare.txt"
  [ -f "$run_dir/manifest-detailed-compare.txt" ] && printf 'manifest_detailed_compare=%s\n' "$run_dir/manifest-detailed-compare.txt"
  [ -f "$run_dir/dictionary-compare.txt" ] && printf 'dictionary_compare=%s\n' "$run_dir/dictionary-compare.txt"
  [ -f "$run_dir/user-dictionary-compare.txt" ] && printf 'user_dictionary_compare=%s\n' "$run_dir/user-dictionary-compare.txt"
  [ -f "$run_dir/public-header-compare.txt" ] && printf 'public_header_compare=%s\n' "$run_dir/public-header-compare.txt"
  [ -f "$run_dir/api-smoke/summary.txt" ] && printf 'api_smoke=%s\n' "$run_dir/api-smoke/summary.txt"
  [ -f "$run_dir/api-callback-smoke/summary.txt" ] && printf 'api_callback_smoke=%s\n' "$run_dir/api-callback-smoke/summary.txt"
  [ -f "$run_dir/warning-budget.txt" ] && printf 'warning_budget=%s\n' "$run_dir/warning-budget.txt"
  [ -f "$run_dir/warning-budget-strict.txt" ] && printf 'warning_budget_strict=%s\n' "$run_dir/warning-budget-strict.txt"
} > "$run_dir/summary.txt"

cat "$run_dir/summary.txt"
