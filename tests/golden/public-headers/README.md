# Public Header Baseline

This directory records the accepted Linux public-header audit.

Regenerate and compare with:

```sh
tools/baseline/check_public_headers.sh \
  --out baseline-runs/current/public-headers \
  --expected tests/golden/public-headers
```

The installed header list reflects the Autotools `src/Makefile.in` install
target. Some installed phoneme headers intentionally do not self-compile without
legacy project macros, so they are recorded separately instead of rewritten.
