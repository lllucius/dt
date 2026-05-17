# Warning Budgets

This directory records warning budgets for categories that have already been
cleaned and are safe to ratchet.

Budgets are intentionally narrow. They should not be used to make all legacy
warnings fatal.

Run locally with:

```sh
tools/baseline/summarize_warnings.py \
  --log baseline-runs/current/build/build.log \
  --out-dir baseline-runs/current/warnings-default
tools/baseline/check_warning_budgets.py \
  --warnings baseline-runs/current/warnings-default/warnings.tsv \
  --budget tests/golden/warnings/default-cleaned.tsv \
  --out baseline-runs/current/warning-budget.tsv
```
