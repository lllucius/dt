# API Boundary Warnings

Phase 5 reviewed API-boundary warnings without changing API source. The goal was
to identify ABI-sensitive cleanup candidates while preserving public headers and
exported symbols.

## Source Report

- Input report:
  `baseline-runs/phase4-tools-samples/warnings-strict/warnings.tsv`
- Relevant prefix:
  `src/dapi/src/api/`

The report uses normalized build-context paths. Some entries under the API build
directory correspond to source compiled from neighboring implementation areas,
such as kernel support files.

## Paths And Types

| Path | Count | Warning types |
| --- | ---: | --- |
| `src/dapi/src/api/ttsapi.c` | 677 | `-Wcast-function-type`, `-Wcast-qual`, `-Wdiscarded-qualifiers`, `-Wimplicit-fallthrough=`, `-Wmaybe-uninitialized`, `-Wmisleading-indentation`, `-Wmissing-prototypes`, `-Wpointer-sign`, `-Wtype-limits`, `-Wunused-but-set-variable`, `-Wunused-parameter`, `-Wunused-variable`, `-Wuse-after-free` |
| `src/dapi/src/api/coop.h` | 66 | `-Wdiscarded-qualifiers` |
| `src/dapi/src/api/init.c` | 60 | `-Wmissing-prototypes`, `-Wunused-variable` |
| `src/dapi/src/api/services.c` | 28 | `-Wmissing-prototypes`, `-Wpointer-sign`, `-Wunused-parameter` |
| `src/dapi/src/api/usa_init.c` | 21 | `-Wcast-qual`, `-Wmaybe-uninitialized`, `-Wmissing-prototypes` |

## Cleanup Decision

No API implementation cleanup was performed in Phase 5.

Reasons:

- `ttsapi.c` and `init.c` are listed as high-risk behavior and ABI areas.
- Many missing-prototype warnings are on exported, reserved, or callback-facing
  API functions; changing linkage or prototypes could affect exported symbols or
  public behavior.
- `coop.h` warnings are qualifier-boundary warnings and should be handled with
  the medium-risk conversion/qualifier cleanup work, not mixed into the
  low-risk wave.
- `ttsapi.h` and `tts.h` were intentionally left unchanged.

Phase 13 and Phase 14 should revisit the remaining warnings with extra-high
reasoning, expanded verification, and explicit exported-symbol/public-header
review.

## Accelerated Plan Phase 11 Pilot

Phase 11 selected the `src/dapi/src/kernel/services.c`
`-Wmissing-prototypes` cluster reported through the API build context as
`src/dapi/src/api/services.c`.

The implementation added matching local prototypes above the existing
definitions. It did not make any symbol `static`, change public headers, change
calling conventions, change structure layout, or alter call sites. The cleanup
therefore preserves the existing external linkage used by command, phoneme,
SAPI, and language-library code.

Strict warning evidence:

- before: `baseline-runs/next3-phase11-warning-strict-pre/warnings.tsv`
- after: `baseline-runs/next3-phase11-api-boundary-services/warnings-strict/`
- result: no remaining `-Wmissing-prototypes` rows for
  `src/dapi/src/kernel/services.c`

Verification:

- `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase11-api-boundary-services --expected tests/golden`
  completed all captures but stopped at the expected detailed manifest
  metadata-hash comparison because the touched source is linked into installed
  language libraries and sample demos.
- The detailed manifest baseline was refreshed after symbol, dictionary,
  public-header, API-smoke, warning-budget, one-shot US audio, and expanded US
  audio comparisons passed.
- Public exported symbol comparisons remained exact. Public headers remained
  unchanged. Generated dictionaries and user dictionaries remained byte-exact.
  API smoke and deterministic US audio remained exact.
- Final current-tree verification passed with
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase11-api-boundary-services-lf --expected tests/golden`.

Remaining API-boundary warning work stays deferred:

- `ttsapi.c` warnings are still high risk because many are exported, reserved,
  callback-facing, or loader-adjacent.
- `init.c` unused locals remain in a high-risk API initialization path.
- Pointer-sign and unused-parameter warnings in `services.c` were not mixed
  into this prototype cleanup.

## Next High-Risk Plan Phase 6 Coop Qualifier Cleanup

Phase 6 selected the `src/dapi/src/api/coop.h` `-Wdiscarded-qualifiers`
cluster from the refreshed strict warning inventory.

The implementation changed only literal-backed global pointer declarations from
`LPSTR` to `const char *`:

- `szLocalMachineDECtalk`
- `szCurrentUsersDECtalk`
- `szMainDictDef`
- `szUserDictDef`
- `szAbbrDictDef`
- `szForeignDictDef`

These variables remain global data symbols with the same names. The change does
not make them `static`, does not change public headers, does not change
dictionary names, and does not alter call sites that copy or format the strings.

Warning evidence:

- before: 66 parser-visible strict `src/dapi/src/api/coop.h`
  `-Wdiscarded-qualifiers` rows.
- after: 0 parser-visible strict `src/dapi/src/api/coop.h`
  `-Wdiscarded-qualifiers` rows.
- strict warning-line count decreased from 29,665 to 29,593.
- parser-visible strict warnings decreased from 29,642 to 29,572.
- `tests/golden/warnings/strict-cleaned.tsv` now enforces the cleaned
  `coop.h` category at zero.

Verification:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next4-phase6-coop-const-final2 \
  --expected tests/golden
```

Result: public headers, exported symbols, detailed manifest, dictionaries, user
dictionaries, API smoke, callback smoke, US audio, expanded US audio suites,
non-US audio, default warning budget, and strict warning budget all matched
accepted baselines.

Remaining API-boundary warning work stays deferred:

- `ttsapi.c` warnings are still high risk because many are exported, reserved,
  callback-facing, loader-adjacent, memory-owner, or file-output adjacent.
- `init.c` unused locals remain in a high-risk API initialization path.
- Pointer-sign and unused-parameter warnings in `services.c` were not mixed
  into this qualifier cleanup.

## Next High-Risk Plan Phase 7 API Implementation Pilot

Phase 7 re-reviewed the refreshed strict warning inventory for
`src/dapi/src/api/ttsapi.c`, `src/dapi/src/api/init.c`, and adjacent exported API
implementation helpers after the expanded API callback and non-US audio gates
were in place.

No API implementation warning cleanup was performed in Phase 7.

Reviewed candidate groups:

- `init.c` `-Wunused-variable` rows are in `__init_shared_mem()` and
  `__fini_shared_mem()`. The locals themselves are unused, but the functions are
  loader/init/fini lifecycle code with historical shared-memory and mutex
  branches, so this phase deferred them rather than mixing lifecycle cleanup
  into the API pilot.
- `ttsapi.c` private-looking `-Wmissing-prototypes` rows for
  `PutIndexMarkInBuffer`, `PutPhonemeInBuffer`, and `WriteAudioToFile` were
  rejected for this phase because the symbols are exported from the language
  shared libraries and are called from VTM output paths. They are directly
  adjacent to index-mark callbacks, phoneme buffering, and deterministic WAV
  output.
- `ttsapi.c` reserved and public API functions such as
  `TextToSpeechControlPanel`, `TextToSpeechGetLastError`,
  `TextToSpeechReserved1`, `TextToSpeechReserved2`, `TextToSpeechReserved3`,
  `TextToSpeechTuning`, `TextToSpeechVisualMarks`, and
  `TextToSpeechReserved5` remain deferred because they are exported, reserved,
  public-header, sample, SAPI, or multi-language dispatch surfaces.
- `ttsapi.c` thread startup, pipe, callback, pointer-sign, discarded-qualifier,
  cast-function-type, use-after-free, and type-limit warnings remain deferred
  because they touch threading, callbacks, memory ownership, loader dispatch, or
  ABI-sensitive conversion behavior.

Decision:

- Treat the remaining `ttsapi.c` and `init.c` warnings as dedicated future work,
  not as a mixed pilot cleanup.
- Do not add a warning-budget row for Phase 7 because no warning category was
  cleaned to zero.
- Preserve public headers, exported symbols, dictionaries, deterministic audio,
  callback behavior, queue behavior, threading behavior, and language selection
  behavior by leaving source unchanged.
