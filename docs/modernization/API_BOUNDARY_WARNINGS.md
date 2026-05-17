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

Phase 13 and Phase 14 should revisit these warnings with extra-high reasoning,
expanded verification, and explicit exported-symbol/public-header review.
