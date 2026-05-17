# Exported Symbol Baseline

This directory records the accepted defined dynamic symbols for every shared
library installed into `dist/lib/` by the Linux Autotools build.

Symbol changes are API/ABI-relevant. Regenerate with:

```sh
tools/baseline/capture_symbols.sh --out baseline-runs/current/symbols
tools/baseline/compare_symbols.sh --expected tests/golden/symbols --actual baseline-runs/current/symbols
```
