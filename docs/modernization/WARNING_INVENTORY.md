# Warning Inventory

This inventory summarizes the warning logs captured during Phase 2 and turns
them into a cleanup order. It is planning documentation only; Phase 3 did not
change source behavior.

## Source Logs

- Default build log:
  `baseline-runs/phase2-golden-current-2/build/build.log`
- Strict warning build log:
  `baseline-runs/phase2-golden-current-2/build/build-strict-warnings.log`
- Parser reports:
  `baseline-runs/phase3-warning-taxonomy/default/`
  `baseline-runs/phase3-warning-taxonomy/strict/`

The raw warning-line counts from the Phase 2 verification run were 1,828 for
the default build and 29,853 for the strict warning build. The parser records
canonical GCC warning locations and therefore counted 1,808 default warnings
and 29,828 strict warnings. The difference is expected for inventory use
because the build logs also contain non-canonical warning text and continuation
lines.

## Strict Warning Totals

Risk classification from `strict/by-risk.tsv`:

| Risk | Count | Ownership |
| --- | ---: | --- |
| high | 9,523 | behavior-critical synthesis, phoneme, LTS, API, and audio/threading areas |
| medium | 4,985 | pointer qualifier, conversion, callback, and related boundary warnings |
| low | 1,850 | unused parameters, missing prototypes, old-style definitions, and format cleanup |
| unknown | 13,470 | unclassified warnings requiring local review before cleanup |

Top strict warning flags from `strict/by-flag.tsv`:

| Count | Flag |
| ---: | --- |
| 12,620 | `-Wpointer-sign` |
| 6,168 | `-Wdiscarded-qualifiers` |
| 3,623 | `-Wcast-qual` |
| 1,745 | `-Wunused-parameter` |
| 1,370 | `-Wmissing-braces` |
| 1,223 | unflagged |
| 925 | `-Wmissing-prototypes` |
| 597 | `-Wunused-variable` |
| 418 | `-Wunused-but-set-variable` |
| 155 | `-Wnested-externs` |

Top strict categories from `strict/by-category.tsv`:

| Count | Category |
| ---: | --- |
| 10,996 | pointer targets in initialization differ in signedness |
| 5,079 | initialization discards qualifier from pointer target type |
| 3,623 | cast discards qualifier from pointer target type |
| 1,745 | unused parameter |
| 1,523 | pointer targets in passing argument differ in signedness |
| 1,370 | missing braces around initializer |
| 1,211 | inline function declared but never defined |
| 925 | no previous prototype |
| 798 | passing argument discards qualifier from pointer target type |
| 569 | unused variable |

## High-Risk Ownership

Warnings in these paths are owned by later high-caution phases and should not be
used as early cleanup targets:

- `src/dapi/src/hlsyn/`
- `src/dapi/src/lts/`
- `src/dapi/src/ph/`
- `src/dapi/src/vtm/`
- `src/dapi/src/api/ttsapi.c`
- `src/dapi/src/api/init.c`
- `src/dapi/src/nt/opthread.c`
- `src/dapi/src/nt/linux_audio.c`

These paths include phoneme logic, language-to-speech rules, synthesizer timing,
public API implementation, and runtime audio/threading behavior. Cleanup here
requires exact audio, symbol, dictionary, and API checks, and some categories
should wait for the extra-high reasoning checkpoints in later phases.

## First Cleanup Wave

Phase 4 should start with warnings outside synthesis-critical paths:

- `src/samplosf/src/dtsamples/`
- `src/dtalkml/src/`
- `src/licunix/src/`
- `src/udicunix/src/`
- private command/tool files under `src/dapi/src/cmd/`

Allowed warning categories for the first wave:

- `-Wunused-parameter`, using explicit `(void)` markers.
- `-Wunused-variable` and `-Wunused-function`, only when removal is local and
  does not affect build outputs.
- `-Wmissing-prototypes` or `-Wmissing-declarations` in private `.c` files.
- `-Wold-style-definition` in private tools and samples.
- obvious `-Wformat` fixes where argument type and formatting intent are clear.
- missing standard includes where the needed declaration is unambiguous.

Avoid in Phase 4:

- pointer signedness or qualifier changes in synthesis, parser, dictionary,
  public API, thread, or audio paths.
- struct layout changes.
- generated dictionary files.
- public headers or exported symbols.

## Later Cleanup Waves

Phase 5 may address API-boundary implementation cleanup, but public headers and
exported symbols must remain byte-for-byte and symbol-for-symbol stable unless a
later phase explicitly approves a baseline change.

Medium-risk warnings should be deferred until Phase 13. In particular,
`-Wcast-qual`, `-Wdiscarded-qualifiers`, callback signature warnings,
pointer/integer conversions, `volatile`, and concurrency-adjacent warnings
should not be mixed into low-risk cleanup commits.

High-risk warnings in phoneme, LTS, VTM, HLSYN, API, audio, and threading paths
require the expanded verification gates from later phases before any cleanup is
attempted.

## Warning Budget Ratchet

Phase 17 adds a narrow warning-budget check for categories that have already
been cleaned. The first budget is:

- `src/dapi/src/dic/dic_comm.c`, `-Wformat=`, maximum count `0`.

This prevents regression of the Phase 13 dictionary compiler format cleanup
without making unrelated legacy warning debt fatal. New budgets should be added
only after a focused cleanup has passed the relevant behavior checks.
