# Dictionary Baseline

Dictionary outputs are behavior-relevant. Phase 6 expands the baseline from
installed main dictionaries to include a deterministic US user dictionary
compiler fixture.

## Main Dictionaries

Committed expected captures:

- `tests/golden/dictionaries/files.txt`
- `tests/golden/dictionaries/sha256.txt`
- `tests/golden/dictionaries/sizes.tsv`

Generated source inputs are under `src/dapi/src/dic/`. The installed outputs are
`dist/dic/*.dic`, including the language-specific and variant dictionaries for
US, UK, French, German, Castilian Spanish, and Latin American Spanish.

Capture and compare:

```sh
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/current/dictionaries
```

## User Dictionary Fixture

Committed fixture input:

- `tests/golden/dictionaries/user/input/us_custom_pronunciations.tab`

Committed expected capture:

- `tests/golden/dictionaries/user/expected/files.txt`
- `tests/golden/dictionaries/user/expected/sha256.txt`
- `tests/golden/dictionaries/user/expected/sizes.tsv`
- `tests/golden/dictionaries/user/expected/logs/us_custom_pronunciations.log`

The fixture is compiled by `dist/tools/udic_us` into a `.dtu` user dictionary.
The binary output is not committed; the accepted filename, size, SHA-256 hash,
and compiler log are committed.

Capture and compare:

```sh
tools/baseline/capture_user_dictionaries.sh --out baseline-runs/current/user-dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries/user/expected --actual baseline-runs/current/user-dictionaries
```

## Current Boundaries

- Do not edit dictionary source text as incidental warning cleanup.
- Do not normalize dictionary source encoding or line endings without an
  explicit baseline update.
- Treat `.dic` and `.dtu` hash changes as behavior changes until reviewed.
- User dictionary fixture coverage is intentionally small and US English only.
