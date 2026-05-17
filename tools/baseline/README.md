# Baseline Tools

These scripts reproduce the local baseline checks without changing existing
build systems.

Typical use:

```sh
tools/baseline/build_unix.sh --strict-warnings --run-dir baseline-runs/current
tools/baseline/capture_dist_manifest.sh --out baseline-runs/current/dist-manifest.txt
tools/baseline/capture_symbols.sh --out baseline-runs/current/symbols
tools/baseline/capture_audio.sh --out baseline-runs/current/audio-us
tools/baseline/compare_audio.py --actual baseline-runs/current/audio-us
```

Generated logs and comparison artifacts should stay under ignored
`baseline-runs/`.
