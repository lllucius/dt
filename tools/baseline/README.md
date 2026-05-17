# Baseline Tools

These scripts reproduce the local baseline checks without changing existing
build systems.

Typical use:

```sh
tools/baseline/build_unix.sh --strict-warnings --run-dir baseline-runs/current
tools/baseline/capture_dist_manifest.sh --out baseline-runs/current/dist-manifest.txt
tools/baseline/capture_dist_manifest.sh \
  --format metadata-hash \
  --out baseline-runs/current/dist-manifest-detailed.txt
tools/baseline/capture_symbols.sh --out baseline-runs/current/symbols
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/capture_user_dictionaries.sh --out baseline-runs/current/user-dictionaries
tools/baseline/capture_audio.sh --out baseline-runs/current/audio-us
tools/baseline/capture_audio_suites.sh --out baseline-runs/current/audio-us-suites
tools/baseline/check_public_headers.sh \
  --out baseline-runs/current/public-headers \
  --expected tests/golden/public-headers
tools/baseline/check_api_smoke.sh --out baseline-runs/current/api-smoke
tools/baseline/compare_audio.py \
  --actual baseline-runs/current/audio-us \
  --metrics-out baseline-runs/current/audio-metrics.tsv
tools/baseline/compare_audio_suites.sh \
  --actual baseline-runs/current/audio-us-suites \
  --metrics-out baseline-runs/current/audio-suite-metrics
```

Single-command verification:

```sh
tools/baseline/verify_current.sh --run-dir baseline-runs/current --expected baseline-runs/accepted
```

The expected directory is optional. When supplied, it may contain `symbols/`,
`dist-manifest.txt`, `dictionaries/`, and `dictionaries/user/expected/`
captures from a previously accepted run.

Warning inventory reports:

```sh
tools/baseline/summarize_warnings.py \
  --log baseline-runs/current/build/build-strict-warnings.log \
  --out-dir baseline-runs/current/warnings-strict
```

The warning summarizer writes TSV reports grouped by flag, category, file, and
risk. It is an inventory helper only; it does not decide whether a warning is
safe to clean.

Warning budget ratchet:

```sh
tools/baseline/summarize_warnings.py \
  --log baseline-runs/current/build/build.log \
  --out-dir baseline-runs/current/warnings-default
tools/baseline/check_warning_budgets.py \
  --warnings baseline-runs/current/warnings-default/warnings.tsv \
  --budget tests/golden/warnings/default-cleaned.tsv \
  --out baseline-runs/current/warning-budget.tsv
```

Warning budgets are narrow checks for already-cleaned files/categories. They do
not make all legacy warnings fatal.

Public header audit:

```sh
tools/baseline/check_public_headers.sh \
  --out baseline-runs/current/public-headers \
  --expected tests/golden/public-headers
```

The header audit compares the accepted installed-header list and syntax-checks
the subset of public or ABI-sensitive headers that currently self-compile.

Public API smoke coverage:

```sh
tools/baseline/check_api_smoke.sh --out baseline-runs/current/api-smoke
```

The smoke test compiles a small C program against `dist/include` and `dist/lib`,
then exercises public startup, US language selection, speaker selection,
speak-to-WAV, sync, wave close, and shutdown behavior without opening live
audio. The generated WAV is compared byte-for-byte against the accepted US
speaker 0 golden audio.

CMake subset verification:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/current-cmake \
  --expected tests/golden
```

This verifies the current side-by-side CMake subset without promoting it to the
primary build path. Packaging-layout differences remain documented separately.

Detailed packaging manifests:

```sh
tools/baseline/capture_dist_manifest.sh \
  --format metadata-hash \
  --out baseline-runs/current/dist-manifest-detailed.txt
tools/baseline/compare_manifest.sh \
  --expected tests/golden/dist-manifest-detailed.txt \
  --actual baseline-runs/current/dist-manifest-detailed.txt \
  --out baseline-runs/current/dist-manifest-detailed.diff
```

The default manifest format remains path/type-oriented for compatibility with
earlier accepted baselines. Use `--format metadata` or
`--format metadata-hash` when packaging work may affect file modes, symlinks, or
payload bytes.

Hosted CI should avoid comparing runner-specific binary addresses, sizes, and
hashes as behavior. Use `tools/baseline/compare_symbols.sh --mode name-type`
for exported symbol type/name checks, and
`tools/baseline/compare_manifest.sh --mode path-type-subset` when the expected
dist paths and file types must be present but optional runner-built tools may
add extra files. The default modes remain exact for local reproducibility gates.

Expanded US audio suites:

```sh
tools/baseline/capture_audio_suites.sh --out baseline-runs/current/audio-us-suites
tools/baseline/compare_audio_suites.sh \
  --actual baseline-runs/current/audio-us-suites \
  --metrics-out baseline-runs/current/audio-suite-metrics
```

The expanded suites use additional committed US English inputs and preserve the
same speaker 0 through 8 WAV comparison policy as the original one-shot
baseline.

Generated logs and comparison artifacts should stay under ignored
`baseline-runs/`.
