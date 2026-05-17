# Baseline Tools

These scripts reproduce the local baseline checks without changing existing
build systems.

Typical use:

```sh
tools/baseline/build_unix.sh --strict-warnings --run-dir baseline-runs/current
tools/baseline/capture_dist_manifest.sh --out baseline-runs/current/dist-manifest.txt
tools/baseline/capture_symbols.sh --out baseline-runs/current/symbols
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/capture_audio.sh --out baseline-runs/current/audio-us
tools/baseline/compare_audio.py --actual baseline-runs/current/audio-us
```

Single-command verification:

```sh
tools/baseline/verify_current.sh --run-dir baseline-runs/current --expected baseline-runs/accepted
```

The expected directory is optional. When supplied, it may contain `symbols/`,
`dist-manifest.txt`, and `dictionaries/` captures from a previously accepted run.

Generated logs and comparison artifacts should stay under ignored
`baseline-runs/`.
