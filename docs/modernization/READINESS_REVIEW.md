# Modernization Readiness Review

Phase 19 records the final readiness state for the current modernization plan.
It is a behavior-preservation report for the Linux target, not approval to
change speech output, public APIs, dictionary formats, audio behavior, or
historical target code.

Later sections record follow-on readiness reviews for subsequent modernization
plans. Each review is limited to the checks that were actually run.

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

## Follow-On Plan Phase 1 Baseline

The next modernization plan started with a post-merge baseline refresh after
PR #1 was merged to `origin/develop`.

Autotools/Linux refresh:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next-phase1-post-merge \
  --expected tests/golden
```

Results:

- default warning-line count: 1,804.
- strict warning-line count: 29,815.
- parser-visible default warnings: 1,783.
- US English golden WAV output matched exactly for speakers 0 through 8.
- exported symbols matched the committed symbol baselines.
- generated main dictionaries matched the committed dictionary baselines.
- generated US user-dictionary fixture output matched the committed expected
  capture.
- public header audit matched the committed allowlists.
- warning budget status was `ok`; `src/dapi/src/dic/dic_comm.c` stayed at zero
  `-Wformat=` warnings.
- the current basic Autotools dist manifest was captured with 589 entries.

CMake subset refresh:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next-phase1-post-merge-cmake \
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

The one-line decrease in default warning count compared with Phase 19 was
observed after the PR merge and rebase; no source behavior change was made in
this refresh phase.

## Follow-On Plan Final Readiness Review

Date: 2026-05-17.

The follow-on plan is complete after Phase 11. The completed work improves
deterministic verification, public API smoke coverage, CMake staging evidence,
and platform-wrapper decision records while keeping Autotools authoritative for
Linux. This review does not approve speech-output changes, public API changes,
dictionary format changes, live-audio behavior changes, or deletion of
historical target code.

Autotools/Linux readiness gate:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next-phase11-readiness \
  --expected tests/golden
```

Results:

- default warning-line count: 1,793.
- strict warning-line count: 29,779.
- parser-visible default warnings: 1,772.
- warning budget status was `ok`; `src/dapi/src/dic/dic_comm.c` stayed at zero
  `-Wformat=` warnings and `src/samplosf/src/dtsamples/mfg_load.c` stayed at
  zero `-Wold-style-definition` warnings.
- US English one-shot golden WAV output matched exactly for speakers 0 through
  8.
- expanded deterministic US audio suites matched exactly for speakers 0 through
  8: `us_abbreviations`, `us_commands_markup`, and
  `us_punctuation_numbers`.
- generated main dictionaries matched the committed dictionary baselines.
- generated US user-dictionary fixture output matched the committed expected
  capture.
- public header audit matched the committed allowlists.
- the public API smoke program built against installed headers and libraries,
  ran through no-audio startup, selected US English speaker 0, wrote a WAV file,
  and matched the committed speaker 0 golden WAV exactly.
- exported symbols matched the committed symbol baselines.
- the Autotools dist manifest matched the committed detailed manifest baseline
  and contained 589 basic manifest entries.

CMake subset readiness gate:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next-phase11-cmake \
  --expected tests/golden
```

Results:

- CMake configured and built `dectalk_cmake_stage`.
- `compile_commands.json` was generated with 3,127 lines.
- CMake-generated dictionaries matched the committed dictionary baselines.
- CMake-staged US English one-shot WAV output matched exactly for speakers 0
  through 8.
- CMake-staged expanded US audio suites matched exactly for speakers 0 through
  8.
- CMake-staged `libtts.so` matched the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets matched the committed
  baselines.
- CMake still remains a side-by-side verification path, not the primary Linux
  build path. Phase 10 documented the remaining staged-layout gap: 589
  Autotools paths, 530 CMake paths, 59 missing Autotools paths, and 0 extra
  CMake paths.

Review areas:

- Linux build reproducibility: the authoritative Autotools/Linux gate rebuilt
  and reproduced accepted behavior baselines under `tests/golden/`.
- CI reliability: the local scripts that CI uses passed here. Hosted PR CI uses
  stable symbol type/name and manifest path/type checks because exact dynamic
  symbol addresses, binary metadata, and optional runner-built tools vary by
  GitHub runner; the local readiness gates keep exact comparisons. The CMake
  subset verifier keeps exact `libtts.so` symbol comparison by default and uses
  type/name comparison only when hosted CI passes `--symbol-mode name-type`.
- warnings: warning counts are lower than the follow-on Phase 1 baseline, but
  warning debt remains substantial and only the narrow warning budget is
  enforced.
- public API and exports: header allowlists, exported-symbol captures, and the
  installed public API smoke test all passed. This is not a full API
  conformance suite.
- dictionaries: main dictionaries and the US user-dictionary fixture are
  reproducible for the committed fixture coverage.
- golden audio: deterministic US English WAV baselines passed for the one-shot
  input and expanded suites. Live audio hardware, callbacks, device selection,
  and timing were not exercised.
- CMake: CMake is useful for side-by-side Linux analysis and deterministic
  subset verification, but it is not ready to replace Autotools.
- platform abstraction: `src/platform/` remains isolated scaffolding and smoke
  coverage. Phase 8 explicitly deferred runtime wrapper wiring because the
  wrappers are not drop-in replacements for legacy runtime semantics.
- historical targets: non-current targets remain preserved and quarantined
  behind explicit legacy options where this plan touched build logic. They were
  not validated as working targets.

Known limitations:

- Not all warnings are addressed.
- High-risk warning cleanup in synthesis, phoneme, LTS, VTM, HLSYN, public API,
  threading, and audio code remains deferred.
- CMake packaging parity remains incomplete.
- Live audio hardware behavior, callback timing, and backend device selection
  were not tested.
- Golden audio coverage remains US English only.
- Non-current platform builds were not tested.
- Behavior preservation is claimed only for the deterministic checks listed in
  this section.

## Next Plan Phase 1 Baseline

The next modernization plan started with a post-merge baseline refresh after
PR #3 was merged to `origin/develop`.

Autotools/Linux refresh:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next2-phase1-post-merge \
  --expected tests/golden
```

Results:

- default warning-line count: 1,793.
- strict warning-line count: 29,779.
- parser-visible default warnings: 1,771.
- warning budget status was `ok`; `src/dapi/src/dic/dic_comm.c` stayed at zero
  `-Wformat=` warnings and `src/samplosf/src/dtsamples/mfg_load.c` stayed at
  zero `-Wold-style-definition` warnings.
- US English one-shot golden WAV output matched exactly for speakers 0 through
  8.
- expanded deterministic US audio suites matched exactly for speakers 0 through
  8: `us_abbreviations`, `us_commands_markup`, and
  `us_punctuation_numbers`.
- exported symbols matched the committed symbol baselines.
- generated main dictionaries matched the committed dictionary baselines.
- generated US user-dictionary fixture output matched the committed expected
  capture.
- public header audit matched the committed allowlists.
- public API smoke output matched the committed speaker 0 golden WAV exactly.
- the detailed Autotools dist manifest matched the committed baseline; the
  basic manifest contained 589 entries.

CMake subset refresh:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next2-phase1-post-merge-cmake \
  --expected tests/golden
```

Results:

- CMake configured and built `dectalk_cmake_stage`.
- `compile_commands.json` was generated with 3,127 lines.
- CMake-generated dictionaries matched the committed dictionary baselines.
- CMake-staged US English one-shot WAV output matched exactly for speakers 0
  through 8.
- CMake-staged expanded US audio suites matched exactly for speakers 0 through
  8.
- CMake-staged `libtts.so` matched the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets matched the committed
  baselines.
- the detailed CMake staged manifest contained 1,041 lines, so CMake remains a
  side-by-side path pending the planned parity work.

The headline default and strict warning-line counts matched the follow-on final
readiness review. The parser-visible default warning count decreased from 1,772
to 1,771 during the post-merge refresh; no source behavior change was made in
this phase.

## PR #4 Final Readiness Review

The modernization plan completed Phases 1 through 12 and was published through
PR #4, `Complete current modernization plan`. PR #4 merged into `develop` as
`0555a8fef13d39f7e31ac96bd582ffb05f34db98`.

Autotools/Linux final gate:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next2-phase12-readiness \
  --expected tests/golden
```

Results:

- default warning-line count: 1,778.
- strict warning-line count: 29,764.
- parser-visible default warnings: 1,757.
- warning budget status was `ok`; all cleaned warning rows stayed at zero:
  `src/dapi/src/dic/dic_comm.c` `-Wformat=`,
  `src/samplosf/src/dtsamples/mfg_load.c` `-Wold-style-definition`,
  `src/licunix/src/liceninc.c` `-Wformat-overflow=`, and
  `src/dapi/src/api/ttsapi.c` `-Wmisleading-indentation`.
- public header audit matched the committed allowlists.
- exported symbols matched the committed symbol baselines.
- detailed Autotools manifest matched the committed baseline; the basic
  manifest contained 589 entries.
- generated main dictionaries and the US user-dictionary fixture matched the
  committed dictionary baselines.
- public API smoke built against installed headers and libraries and matched
  the committed speaker 0 WAV exactly.
- US English one-shot golden WAV output matched exactly for speakers 0 through
  8.
- expanded deterministic US audio suites matched exactly for speakers 0 through
  8: `us_abbreviations`, `us_commands_markup`, and
  `us_punctuation_numbers`.

CMake subset final gate:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next2-phase12-readiness-cmake \
  --expected tests/golden
```

Results:

- CMake configured and built `dectalk_cmake_stage`.
- `compile_commands.json` was generated with 3,295 lines.
- CMake-generated dictionaries matched the committed dictionary baselines.
- CMake-staged US English one-shot WAV output matched exactly for speakers 0
  through 8.
- CMake-staged expanded US audio suites matched exactly for speakers 0 through
  8.
- CMake-staged `libtts.so` matched the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets matched the committed
  baselines.
- expanded `dt_platform_smoke` passed with `event_semantics=ok`.
- CMake basic staged manifest path/type parity matched Autotools exactly:
  589 Autotools entries and 589 CMake entries.
- CMake detailed metadata-hash manifest still differs from Autotools detailed
  output because CMake-built binaries and the `doc/DECtalk/html` directory
  metadata differ. CMake remains side-by-side and is not promoted.

Review areas:

- Linux build reproducibility: the authoritative Autotools/Linux gate rebuilt
  and reproduced accepted deterministic baselines under `tests/golden`.
- Hosted and local CI behavior: local CI-equivalent scripts passed before PR
  publication. GitHub accepted the PR #4 merge after hosted checks reached a
  policy-acceptable state.
- warnings: warning counts decreased during the plan, but substantial warning
  debt remains. The warning budget only ratchets categories that have already
  been cleaned.
- public API and exports: public header allowlists, exported-symbol captures,
  and the installed public API smoke test all passed. This is not a full API
  conformance suite.
- dictionaries: main dictionaries and the US user-dictionary fixture are
  reproducible for committed fixture coverage.
- golden audio: deterministic US English WAV baselines passed for the one-shot
  input and expanded suites. Live audio hardware, callbacks, backend device
  selection, and timing were not exercised.
- phoneme/text baselines: no phoneme or text-mode golden baseline was added.
  Phase 3 documented current capture blockers.
- CMake: CMake now has exact basic path/type staged-layout parity with
  Autotools, but detailed metadata/hash differences and live-audio option
  parity still block promotion.
- platform abstraction: `src/platform/` remains isolated scaffolding. Phase 9
  expanded CMake-only wrapper smoke coverage, and Phase 10 deferred runtime
  wrapper wiring because legacy `OP_*` and live-audio semantics are not proven.
- historical target and macro quarantine: Phase 11 clarified current Linux,
  sound-critical, product, CMake-parity, platform-wrapper, and historical-target
  macro groups. No historical target code was deleted or validated as current.

Known limitations:

- Not all warnings are addressed.
- High-risk warning cleanup in synthesis, phoneme, LTS, VTM, HLSYN, public API,
  threading, and audio code remains deferred.
- CMake is not promoted to the primary Linux build path.
- CMake does not yet expose the Autotools live-audio backend option surface.
- Live audio hardware behavior, callback timing, queue behavior, and backend
  device selection were not tested.
- Golden audio coverage remains US English only.
- Non-current platform builds were not tested.
- Historical target branches are preserved and documented, not proven working.
- Behavior preservation is claimed only for the deterministic checks listed in
  this section.

## Accelerated Plan Phase 1 Baseline

The accelerated modernization plan started with a post-merge baseline refresh
after PR #4 was merged to `origin/develop` as
`0555a8fef13d39f7e31ac96bd582ffb05f34db98`. Local `develop` also contained the
new planning commit `bf6e56912a5fb9c40de35b11b721c5e42cf0255b`.

Autotools/Linux refresh:

```sh
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next3-phase1-post-merge \
  --expected tests/golden
```

Results:

- default warning-line count: 1,778.
- strict warning-line count: 29,762.
- parser-visible default warnings: 1,757.
- warning budget status was `ok`.
- public header audit matched the committed allowlists.
- exported symbols matched the committed symbol baselines.
- detailed Autotools manifest matched the committed baseline.
- generated main dictionaries and the US user-dictionary fixture matched the
  committed dictionary baselines.
- public API smoke output matched the committed speaker 0 golden WAV exactly.
- US English one-shot golden WAV output matched exactly for speakers 0 through
  8.
- expanded deterministic US audio suites matched exactly for speakers 0 through
  8: `us_abbreviations`, `us_commands_markup`, and
  `us_punctuation_numbers`.

CMake subset refresh:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next3-phase1-post-merge-cmake \
  --expected tests/golden
```

Results:

- CMake configured and built the staged target.
- `compile_commands.json` was generated with 3,295 lines.
- CMake-generated dictionaries matched the committed dictionary baselines.
- CMake-staged US English one-shot WAV output matched exactly for speakers 0
  through 8.
- CMake-staged expanded US audio suites matched exactly for speakers 0 through
  8.
- CMake-staged `libtts.so` matched the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets matched the committed
  baselines.
- expanded `dt_platform_smoke` passed with `event_semantics=ok`.
- the detailed CMake staged manifest contained 1,126 lines.

The default warning-line count and parser-visible default warning count matched
the previous final readiness review. The strict warning-line count decreased
from 29,764 to 29,762 during the post-merge refresh. No source behavior change
was made in this phase.
