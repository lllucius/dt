# Generated Dictionary Baseline

This directory records the accepted Linux Autotools `dist/dic/*.dic` outputs as
file names, sizes, and SHA-256 hashes.

Dictionary changes are behavior-relevant. Regenerate with:

```sh
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/current/dictionaries
```

User dictionary compiler fixtures are under `user/input/`, with accepted
byte-level captures under `user/expected/`. Regenerate with:

```sh
tools/baseline/capture_user_dictionaries.sh --out baseline-runs/current/user-dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries/user/expected --actual baseline-runs/current/user-dictionaries
```
