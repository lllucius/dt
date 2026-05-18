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
- `src/samplosf/src/dtsamples/mfg_load.c`, `-Wold-style-definition`, maximum
  count `0`.

This prevents regression of the Phase 13 dictionary compiler format cleanup
and the Phase 5 sample loader prototype-definition cleanup without making
unrelated legacy warning debt fatal. New budgets should be added only after a
focused cleanup has passed the relevant behavior checks.

## Accelerated Plan Phase 3 Refresh

The accelerated plan refreshed warning evidence from the Phase 1 post-merge
baseline:

- default warning summary:
  `baseline-runs/next3-phase1-post-merge/warnings-default/`
- strict warning log:
  `baseline-runs/next3-phase1-post-merge/build/build-strict-warnings.log`
- refreshed strict parser summary:
  `baseline-runs/next3-phase3-warning-strict/`

Current warning counts:

| Source | Count |
| --- | ---: |
| default warning lines | 1,778 |
| parser-visible default warnings | 1,757 |
| strict warning lines | 29,762 |
| parser-visible strict warnings | 29,743 |

Refreshed strict risk classification:

| Risk | Count | Ownership |
| --- | ---: | --- |
| high | 9,333 | behavior-critical synthesis, phoneme, LTS, API, and audio/threading areas |
| medium | 5,147 | pointer qualifier, conversion, callback, and related boundary warnings |
| low | 1,670 | unused parameters, missing prototypes, old-style definitions, and local tool cleanup |
| unknown | 13,593 | unclassified warnings requiring local review before cleanup |

Selected Phase 4 candidate:

- file: `src/licunix/src/liceninc.c`
- category: `-Wmissing-prototypes`
- current strict parser evidence: three repeated warnings for private helper
  `all_digits`
- planned approach: make the helper internal to the translation unit if review
  confirms it is not externally referenced
- expected budget: add a zero-count `-Wmissing-prototypes` row for
  `src/licunix/src/liceninc.c` only if the category reaches zero after
  verification

Rejected candidates for this pass:

- `src/licunix/src/liceninc.c` `-Wpointer-sign`: pointer signedness is outside
  this low-risk phase and should not be mixed with a prototype cleanup.
- `src/samplosf/src/dtsamples/tunecheck.c` `-Wunused-variable`: larger sample
  tool surface and more warnings; suitable only after a focused review.
- `src/samplosf/src/dtsamples/mfg_load.c` `-Wmissing-prototypes`: already has a
  warning-budget history, but the remaining functions need more local review
  than the single-helper `liceninc.c` target.
- private command/parser files under `src/dapi/src/cmd/`: parser-adjacent and
  deferred until stronger behavior coverage is needed.

Do not expand Phase 4 beyond the selected file and warning category unless the
candidate proves invalid during source review.

## Accelerated Plan Phase 4 Cleanup

Phase 4 implemented the selected `src/licunix/src/liceninc.c`
`-Wmissing-prototypes` cleanup by making the private `all_digits` helper
file-local with `static`.

Verification:

- `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase4-liceninc-warning --expected tests/golden`
- `tools/baseline/summarize_warnings.py --log baseline-runs/next3-phase4-liceninc-warning/build/build-strict-warnings.log --out-dir baseline-runs/next3-phase4-warning-strict`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/next3-phase4-liceninc-warning/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/next3-phase4-liceninc-warning/warning-budget-after.tsv`

Results:

- strict warning-line count decreased from 29,762 to 29,761.
- parser-visible strict warnings decreased from 29,743 to 29,738.
- the refreshed strict parser reported no remaining warnings for
  `src/licunix/src/liceninc.c`.
- the warning budget passed after adding a zero-count
  `src/licunix/src/liceninc.c` `-Wmissing-prototypes` row.
- public headers, exported symbols, dictionaries, user dictionaries, API smoke,
  one-shot US audio, expanded US audio suites, and the detailed Autotools
  manifest matched accepted baselines.

## Next High-Risk Plan Phase 6 Cleanup

Phase 6 implemented one medium-risk warning cleanup:

- file: `src/dapi/src/api/coop.h`
- category: `-Wdiscarded-qualifiers`
- change: literal-backed dictionary and registry path globals now use
  `const char *` instead of mutable `LPSTR`

The selected variables were:

- `szLocalMachineDECtalk`
- `szCurrentUsersDECtalk`
- `szMainDictDef`
- `szUserDictDef`
- `szAbbrDictDef`
- `szForeignDictDef`

Warning evidence:

- before: 66 parser-visible strict `coop.h` `-Wdiscarded-qualifiers` rows.
- after: 0 parser-visible strict `coop.h` `-Wdiscarded-qualifiers` rows.
- strict warning-line count decreased from 29,665 to 29,593.
- parser-visible strict warnings decreased from 29,642 to 29,572.

Budget ratchet:

- `tests/golden/warnings/strict-cleaned.tsv` now tracks
  `src/dapi/src/api/coop.h`, `-Wdiscarded-qualifiers`, maximum count `0`.
- `tools/baseline/verify_current.sh` now emits a strict warning parser summary
  and checks the strict budget when the file is present.

Verification:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next4-phase6-coop-const-final2 \
  --expected tests/golden
```

## Next Warning Cleanup Plan Refresh

The warning-focused follow-on plan refreshed warning evidence with:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next5-phase1-post-pr6 \
  --expected tests/golden
```

Current warning counts:

| Source | Count |
| --- | ---: |
| default warning lines | 1,778 |
| parser-visible default warnings | 1,757 |
| strict warning lines | 29,593 |
| parser-visible strict warnings | 29,572 |

Selected cleanup sequence:

1. `src/udicunix/src/alphabet.c`, strict `-Wunused-variable`.
   The target rows are local unused variables in the user-dictionary
   alphabetizer. The cleanup must not touch pointer signedness, text parsing,
   codepage conversion, sort order, file I/O, dictionary format, or generated
   user-dictionary output.
2. `src/samplosf/src/dtsamples/tunecheck.c`, strict
   `-Wmissing-prototypes`.
   The target row is private helper `MakeTunerParams`. The cleanup may make
   the helper file-local only if source review confirms there is no external
   reference.
3. `src/samplosf/src/dtsamples/tunecheck.c`, strict
   `-Wunused-variable`.
   The target rows are unused locals in the sample tool. The cleanup must not
   touch callback behavior, buffer processing, tuner-string generation,
   command-line parsing, format-y2k output, qualifier cleanup, or pointer
   signedness.

Rejected candidates for this pass:

- `src/udicunix/src/alphabet.c` pointer-sign warnings: these are dictionary
  text-buffer boundary warnings and should not be mixed with unused-variable
  removal.
- `src/samplosf/src/dtsamples/tunecheck.c` `-Wdiscarded-qualifiers` and
  `-Wformat-y2k`: these can affect string typing or displayed output and need
  a separate review.
- parser-adjacent command files under `src/dapi/src/cmd/`: parser behavior is
  higher risk than the selected shipped tool cleanup.
- `src/dapi/src/osf/` stubs: many rows are low-risk-looking unused parameters,
  but those files model compatibility APIs and should be grouped separately.

Budget policy for this plan:

- Add strict warning-budget rows only after a selected file/category reaches
  zero and the full default gate passes.
- Do not broaden budgets to unrelated warning debt.

Result: the final gate passed with public headers, exported symbols, detailed
manifest, dictionaries, user dictionaries, API smoke, callback smoke, US
one-shot audio, expanded US audio suites, non-US one-shot audio, default warning
budget, and strict warning budget all matching accepted baselines.

Notes:

- the detailed manifest changed because the edited header is compiled into the
  language shared libraries and `say_demo_*` tools; the manifest baseline was
  refreshed only after symbols, dictionaries, API, callback, audio, and warning
  gates passed.
- no public API signatures, exported symbol names, dictionary strings, parser
  behavior, synthesis behavior, callback behavior, or audio behavior were
  intentionally changed.

The pointer-sign cleanup originally visible in this file remains out of scope
as a warning category. It should not be mixed into low-risk warning cleanup
unless a later phase explicitly selects it.

## Accelerated Plan Phase 11 API-Boundary Pilot

Phase 11 selected one API-boundary warning candidate from the refreshed strict
warning inventory:

- file: `src/dapi/src/kernel/services.c`
- normalized build-context path in API warnings: `src/dapi/src/api/services.c`
- category: `-Wmissing-prototypes`
- approach: add matching local prototypes for existing externally linked
  service functions without changing linkage, definitions, public headers,
  call sites, or exported symbol names

Results:

- parser-visible default warnings remained `1,757`.
- default warning-line count decreased from `1,778` to `1,777`.
- strict warning-line count decreased from `29,760` to `29,665`.
- parser-visible strict warnings decreased from `29,738` to `29,644`.
- the refreshed strict parser reported no remaining
  `src/dapi/src/kernel/services.c` `-Wmissing-prototypes` warnings.
- a zero-count warning-budget row was added for
  `src/dapi/src/kernel/services.c` `-Wmissing-prototypes`.
- public exported symbols, public headers, generated dictionaries, user
  dictionaries, API smoke output, one-shot US audio, and expanded deterministic
  US audio matched accepted baselines.

The detailed Autotools manifest metadata-hash baseline was refreshed because
`services.c` is linked into installed language libraries and sample demo
binaries. The path/type install layout did not change when compared with the
Phase 10 Autotools capture.

Deferred warning categories remain unchanged: pointer-sign, pointer qualifier,
unused-parameter, callback-facing, loader-adjacent, structure-layout,
threading, and public API implementation warnings still require separate
extra-high review and gates.
