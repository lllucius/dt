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
tools/baseline/check_public_headers.sh \
  --out baseline-runs/current/public-headers \
  --expected tests/golden/public-headers
tools/baseline/compare_audio.py \
  --actual baseline-runs/current/audio-us \
  --metrics-out baseline-runs/current/audio-metrics.tsv
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

Detailed packaging manifests:

```sh
tools/baseline/capture_dist_manifest.sh \
  --format metadata-hash \
  --out baseline-runs/current/dist-manifest-detailed.txt
```

The default manifest format remains path/type-oriented for compatibility with
earlier accepted baselines. Use `--format metadata` or
`--format metadata-hash` when packaging work may affect file modes, symlinks, or
payload bytes.

Generated logs and comparison artifacts should stay under ignored
`baseline-runs/`.
