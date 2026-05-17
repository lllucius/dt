# Modernization Readiness Review

Phase 19 records the final readiness state for the current modernization plan.
It is a behavior-preservation report for the Linux target, not approval to
change speech output, public APIs, dictionary formats, audio behavior, or
historical target code.

## Conclusion

The current modernization plan is complete after Phase 19. The repository now
has reproducible Linux baseline checks, committed golden behavior artifacts,
warning inventory and a narrow warning-budget ratchet, public header and export
audits, dictionary and user-dictionary checks, packaging-layout documentation,
side-by-side CMake subset verification, and documented historical-target
quarantine rules.

This does not mean the broader cleanup is finished. Significant legacy warning
debt remains, CMake is not ready to replace Autotools as the primary Linux build
path, live audio hardware behavior was not tested, and non-current targets were
preserved rather than validated.

## Verification Run

Autotools/Linux readiness gate:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/phase19-readiness \
  --expected tests/golden
```

Results:

- default warning-line count: 1,805.
- strict warning-line count: 29,815.
- parser-visible default warnings: 1,784.
- US English golden WAV output matched exactly for speakers 0 through 8.
- exported symbols matched the committed symbol baselines.
- generated main dictionaries matched the committed dictionary baselines.
- generated US user-dictionary fixture output matched the committed expected
  capture.
- public header audit matched the committed allowlists.
- warning budget status was `ok`; `src/dapi/src/dic/dic_comm.c` stayed at zero
  `-Wformat=` warnings.
- the current basic Autotools dist manifest was captured with 589 entries.

CMake subset readiness gate:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/phase19-readiness-cmake \
  --expected tests/golden
```

Results:

- CMake configured and built `dectalk_cmake_stage`.
- `compile_commands.json` was generated with 3,127 lines.
- CMake-generated dictionaries matched the committed dictionary baselines.
- CMake-staged US English WAV output matched exactly for speakers 0 through 8.
- CMake-staged `libtts.so` exported symbols matched the committed baseline
  exactly.
- CMake language-library exported symbol name/type sets matched the committed
  baselines.
- the detailed CMake staged manifest contained 90 entries, so CMake remains a
  packaging subset.

## Review Areas

### Linux Build Reproducibility

Autotools remains the authoritative Linux build path. The Phase 19 Autotools
gate rebuilt the project, captured artifacts, and reproduced every committed
behavior baseline that currently exists under `tests/golden/`.

### CI Reliability

Ubuntu CI now runs behavior gates for exported symbols, generated dictionaries,
user dictionaries, US English golden audio, public header allowlists, the narrow
warning budget, and the CMake subset verifier. This local review did not run
GitHub-hosted CI; it verifies the workflow definitions and the same local
scripts those jobs call.

### Warning Counts And Policy

Warnings are inventoried and categorized, but they are not all fixed. Current
Phase 19 counts are 1,805 default warning lines and 29,815 strict warning
lines. The enforced policy is intentionally narrow: only warning categories that
have already been cleaned should be added to warning budgets.

### Public Headers And Exports

Installed public headers and self-compiling API-sensitive headers are
allowlisted under `tests/golden/public-headers/`. Exported dynamic symbols are
versioned under `tests/golden/symbols/`. Phase 19 reproduced both checks. No
public API signature, exported symbol name, calling convention, or structure
layout change is approved by this review.

### Dictionary Generation

Main installed dictionaries and the US user-dictionary fixture are reproducible
against committed file lists, sizes, and SHA-256 captures. Coverage is still a
fixture-level guard, not a full dictionary-behavior proof for every possible
user dictionary input.

### Golden Audio Coverage

Golden audio covers US English, speakers 0 through 8, using the committed
one-shot input text and WAV file output. The Phase 19 checks prove exact
reproduction for that coverage. They do not cover live audio devices, every
language, every parser path, or spectrum analysis beyond exact file equality
and recorded metrics.

### CMake Status

CMake is useful as a side-by-side Linux verification and analysis path. It is
not ready to become the primary Linux build path because staged packaging is
still a subset of Autotools, and language-library full address-sorted symbol
captures still differ even though exported symbol name/type sets match.

### Platform Abstraction Status

`src/platform/` contains POSIX time, filesystem, thread, mutex, event, legacy
target, and audio-backend metadata scaffolding. The scaffolding is compiled by
CMake smoke targets, but it is not wired into the DECtalk runtime. Existing
threading, queue, pipe, and audio backend behavior remains owned by the legacy
runtime files.

### Historical Target Quarantine

Historical targets remain preserved in source and build files. The current plan
added explicit opt-in rules for non-current Autotools target triplets and CMake
legacy-target experimentation, but it did not delete or validate historical
Windows, macOS, OSF/Tru64, VxWorks, MS-DOS, Windows CE, Solaris/SPARC, ARM7,
MIPS, old PowerPC Mac, iPAQ Linux, or Emscripten behavior.

## Known Limitations

- The current plan does not address all compiler warnings.
- High-risk warning cleanup in synthesis, phoneme, LTS, VTM, HLSYN, public API,
  threading, and audio code remains deferred.
- CMake packaging parity is not complete.
- Autotools `dist/` manifest capture is available, but there is no committed
  `tests/golden` dist-manifest baseline in this plan.
- Live audio hardware behavior, callback timing, and backend device selection
  were not exercised.
- Golden audio coverage is US English only.
- Non-current platform builds were inventoried and quarantined, not tested.
- Public API behavior is guarded by header and exported-symbol checks, not by a
  full API conformance suite.

## Recommendation

Proceed to higher-risk modernization only with a new phase plan and exact
behavior gates selected before each change. Good next objectives are focused
warning-budget expansions, CMake packaging parity, additional deterministic
audio or phoneme baselines, and isolated API-boundary cleanup. Do not promote
CMake, rewrite audio/threading paths, simplify sound-critical macros, or edit
public headers without explicit approval and matching verification.
