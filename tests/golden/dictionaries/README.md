# Generated Dictionary Baseline

This directory records the accepted Linux Autotools `dist/dic/*.dic` outputs as
file names, sizes, and SHA-256 hashes.

Dictionary changes are behavior-relevant. Regenerate with:

```sh
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/current/dictionaries
```
