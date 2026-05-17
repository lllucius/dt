# PLAN.md

## Objective

Complete the DECtalk modernization while preserving current Linux behavior.

The end state is a codebase that is easier to build, easier to verify, easier to
warning-clean, easier to port, and easier to maintain without accidental changes
to speech output, public API behavior, generated dictionaries, packaging, or
runtime behavior.

## Guiding Principle

Measure behavior first. Refactor second. Retire complexity only after it is
classified, isolated, and covered by checks.

## Existing Knowledge Base

The previous plan established the modernization foundation:

- `docs/modernization/BUILD_OVERVIEW.md`
- `docs/modernization/RISK_AREAS.md`
- `docs/modernization/MACRO_INVENTORY.md`
- `docs/modernization/BASELINE_PROCEDURE.md`
- `docs/modernization/CMAKE_OVERVIEW.md`
- `docs/modernization/HISTORICAL_TARGETS.md`
- `tools/baseline/`
- `tests/golden/input/us_one_shot.txt`
- `tests/golden/audio/us/speaker_0.wav` through `speaker_8.wav`
- `src/platform/`
- initial side-by-side `CMakeLists.txt`

Known baseline facts:

- Autotools default Linux build succeeds.
- Strict warning build succeeds.
- Post-cleanup warning-line counts were last observed as:
  - default build: 1,828
  - strict build: 29,839
- US English golden WAVs for speakers 0 through 8 compare exactly after rebuild.
- Exported symbols matched the Phase 1 symbol baseline after the first cleanup.
- CMake currently builds only initial Linux US `libtts_us.so` and `say_cmake`.
- CMake behavior parity is not yet claimed.

## Operating Rules

- Work phases strictly in order.
- Do not advance while the current phase has failing checks or unresolved
  regressions.
- Keep each subtask small enough to leave the tree buildable and testable.
- Update this plan immediately after each completed phase with an Implementation
  Summary.
- Commit often while executing the plan. Each logical subtask should end in a
  stable, verified commit when feasible. After completing the requested plan
  scope, create a pull request with an appropriate body summarizing changes,
  verification, behavior risk, and limitations, then automatically merge that PR
  once required checks and repository policy allow it.
- Do not delete historical target code without explicit approval.
- Do not change speech output, phoneme output, dictionary behavior, public API
  signatures, exported symbols, install layout, sample rate, default voice,
  threading model, or audio backend behavior unless a phase explicitly approves
  that change and verification proves the result.
- Every newly created source or header file must include standard project
  documentation at the top of the file: purpose, scope, behavior-preservation
  notes, and any important limitations or ownership boundaries. Keep comments
  concise and avoid changing existing file headers unless the phase explicitly
  requires it.
- Execute ordinary phases with high reasoning. At each reasoning escalation
  checkpoint, stop before implementation, call out the need for extra-high
  reasoning, and wait for explicit approval before continuing that phase.
- When extra-high checkpoint work is complete and the next work can return to
  ordinary high reasoning, stop before continuing, call out that the reasoning
  level should change back to high, and wait for explicit approval.

## Reasoning Escalation Checkpoints

Extra-high reasoning is recommended before starting these phases or subtasks:

- Phase 8: CMake parity decisions involving source membership, generated
  dictionaries, staged install layout, or `libtts.so` multi-language behavior.
- Phase 10: any decision to wire thread, mutex, or event wrappers into existing
  runtime code.
- Phase 11: any decision to route audio behavior through new backend
  abstractions.
- Phase 13: medium-risk warning cleanup involving conversions, callback
  signatures, thread function signatures, `volatile`, or concurrency.
- Phase 14: public header, ABI, calling convention, or exported-symbol policy
  decisions.
- Phase 18: CMake promotion recommendation.
- Phase 19: final modernization readiness conclusions and any recommendation to
  proceed to higher-risk objectives.

If any earlier phase finds an unexplained audio, dictionary, symbol, manifest,
or API delta, treat that investigation as an extra-high reasoning checkpoint
even if the phase is not listed above.

## Standard Verification Gate

Every phase that changes code or build behavior must run the narrowest relevant
checks and state the result:

- `tools/baseline/build_unix.sh --strict-warnings --run-dir baseline-runs/<phase>`
- `tools/baseline/capture_symbols.sh --out baseline-runs/<phase>/symbols`
- compare exported symbols against the previous accepted baseline
- `tools/baseline/capture_audio.sh --out baseline-runs/<phase>/audio-us`
- `tools/baseline/compare_audio.py --actual baseline-runs/<phase>/audio-us --metrics-out baseline-runs/<phase>/audio-metrics.tsv`
- capture or compare `dist` manifest when packaging/install behavior might be
  affected
- `git diff --check`, with explicit note if legacy CRLF files require scoped
  exclusion

Do not claim behavior preservation unless the relevant checks were run.

---

## Phase 1: Verification Hardening

Status: completed

Implementation Summary:

- Added baseline comparison and verification scripts:
  - `tools/baseline/compare_symbols.sh`
  - `tools/baseline/compare_manifest.sh`
  - `tools/baseline/capture_dictionaries.sh`
  - `tools/baseline/compare_dictionaries.sh`
  - `tools/baseline/verify_current.sh`
- Updated `tools/baseline/README.md` and
  `docs/modernization/BASELINE_PROCEDURE.md` with the new capture/compare
  workflow.
- Added additional committed US English fixed input texts for future golden
  coverage:
  - `tests/golden/input/us_abbreviations.txt`
  - `tests/golden/input/us_commands_markup.txt`
  - `tests/golden/input/us_punctuation_numbers.txt`
- Verification run:
  - `tools/baseline/capture_symbols.sh --out baseline-runs/phase1-hardening-expected/symbols`
  - `tools/baseline/capture_dist_manifest.sh --out baseline-runs/phase1-hardening-expected/dist-manifest.txt`
  - `tools/baseline/capture_dictionaries.sh --out baseline-runs/phase1-hardening-expected/dictionaries`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase1-hardening-current --expected baseline-runs/phase1-hardening-expected`
- Verification results:
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched the expected capture.
  - dist manifest matched the expected capture.
  - generated dictionary file list, sizes, and SHA-256 hashes matched the
    expected capture.
  - default warning-line count: 1,829.
  - strict warning-line count: 29,854.
- No engine code, public headers, dictionary sources, generated committed audio,
  exported symbols, or install layout were changed.

Goals:

- Convert the existing baseline scripts into a stronger verification harness.
- Add machine-readable comparison reports for symbols, manifests, warning counts,
  and audio.
- Add deterministic dictionary output/hash capture.
- Add broader fixed text inputs before deeper cleanup begins.
- Make it easy to compare a new run against the last accepted baseline without
  hand-written shell loops.

Suggested work:

- Add `tools/baseline/compare_symbols.sh` or equivalent.
- Add `tools/baseline/compare_manifest.sh`.
- Add `tools/baseline/capture_dictionaries.sh`.
- Add `tools/baseline/compare_dictionaries.sh`.
- Add more committed text inputs under `tests/golden/input/`.
- Keep generated logs and comparison outputs under ignored `baseline-runs/`.

Success criteria:

- A single documented command sequence rebuilds, captures, and compares the
  current tree to accepted baselines.
- Symbol comparisons are exact.
- Golden audio comparisons are exact for the existing US speaker set.
- Dictionary hashes are captured and compared.
- No engine code changes are made in this phase.

Rules:

- Do not update golden WAVs unless the current committed WAVs are proven
  incomplete or invalid and the reason is documented.
- Do not make CI fail on warnings yet.

---

## Phase 2: CI Verification Gates

Status: completed

Implementation Summary:

- Added committed exact baselines for CI verification:
  - `tests/golden/symbols/`
  - `tests/golden/dictionaries/`
- Added README files documenting how to regenerate and compare those baselines.
- Updated Ubuntu CI in `.github/workflows/build.yml` with a `Verify baseline
  outputs` step that fails on:
  - exported symbol drift against `tests/golden/symbols`
  - generated dictionary file/hash/size drift against `tests/golden/dictionaries`
  - US English golden audio drift against `tests/golden/audio/us`
- Kept warning logs non-fatal and kept the dist manifest as an artifact/report
  capture rather than a failing gate.
- Updated baseline procedure docs to point comparisons at the committed golden
  symbol and dictionary baselines.
- Verification run:
  - `tools/baseline/capture_symbols.sh --out baseline-runs/phase2-ci-gates/symbols`
  - `tools/baseline/compare_symbols.sh --expected tests/golden/symbols --actual baseline-runs/phase2-ci-gates/symbols --out baseline-runs/phase2-ci-gates/symbol-compare`
  - `tools/baseline/capture_dictionaries.sh --out baseline-runs/phase2-ci-gates/dictionaries`
  - `tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/phase2-ci-gates/dictionaries --out baseline-runs/phase2-ci-gates/dictionary-compare`
  - `tools/baseline/capture_audio.sh --out baseline-runs/phase2-ci-gates/audio-us`
  - `tools/baseline/compare_audio.py --actual baseline-runs/phase2-ci-gates/audio-us`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase2-golden-current-2 --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated dictionary file list, sizes, and SHA-256 hashes matched committed
    baselines.
  - default warning-line count: 1,828.
  - strict warning-line count: 29,853.
- Full GitHub Actions execution was not run locally.
- No engine code, public headers, dictionary sources, generated audio, exported
  symbols, or install layout were intentionally changed.

Goals:

- Move from CI artifact visibility to CI verification.
- Keep warnings non-fatal initially, but fail CI on behavior regressions that
  should be exact.
- Publish concise comparison reports as artifacts.

Suggested work:

- Add Ubuntu CI steps for exact symbol comparison.
- Add Ubuntu CI steps for exact golden audio comparison.
- Add Ubuntu CI steps for dictionary hash comparison.
- Add dist manifest comparison in report-only mode first.
- Keep VS6 and VS2022 jobs present and artifact-producing.

Success criteria:

- CI fails on golden audio mismatch.
- CI fails on exported symbol mismatch for public libraries.
- CI fails on generated dictionary hash mismatch once the dictionary baseline is
  established.
- Warning logs remain published, but warnings are not fatal.

Rules:

- Do not change packaging layout.
- Do not remove existing CI jobs.
- Do not make warning cleanup block unrelated PRs yet.

---

## Phase 3: Warning Taxonomy and Ownership

Status: completed

Implementation Summary:

- Added `tools/baseline/summarize_warnings.py`, a GCC-style warning inventory
  parser that normalizes nested `make` warning paths and writes TSV reports by
  flag, category, file, and risk.
- Added `docs/modernization/WARNING_INVENTORY.md` with strict/default warning
  counts, top warning categories, high-risk ownership boundaries, and the
  intended first cleanup wave.
- Updated `tools/baseline/README.md` with the warning summarizer workflow.
- Verification run:
  - `tools/baseline/summarize_warnings.py --log baseline-runs/phase2-golden-current-2/build/build.log --out-dir baseline-runs/phase3-warning-taxonomy/default`
  - `tools/baseline/summarize_warnings.py --log baseline-runs/phase2-golden-current-2/build/build-strict-warnings.log --out-dir baseline-runs/phase3-warning-taxonomy/strict`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - parser-visible default warnings: 1,808.
  - parser-visible strict warnings: 29,828.
  - strict risk split: 9,523 high, 4,985 medium, 1,850 low, 13,470 unknown.
  - no warning cleanup was performed.
- No engine code, public headers, dictionary sources, generated audio,
  exported symbols, install layout, or runtime behavior were changed.

Goals:

- Turn strict warning output into a category-by-category cleanup queue.
- Separate low-risk warnings from medium-risk and high-risk warnings.
- Identify files that should be cleaned first because they are tools, samples, or
  build-only code rather than synthesis-critical code.

Suggested work:

- Add a warning parser that groups by warning flag, file, and category.
- Produce `docs/modernization/WARNING_INVENTORY.md`.
- Create a cleanup order that starts outside synthesis-critical paths.
- Mark high-risk warnings that should not be touched without additional audio or
  phoneme coverage.

Success criteria:

- Warning categories have owners/risk levels.
- First cleanup wave is clearly scoped.
- No warning cleanup is performed in this phase unless it is needed to validate
  the parser.

Rules:

- Do not edit synthesis arithmetic.
- Do not alter public headers as part of inventory.

---

## Phase 4: Low-Risk Warning Cleanup, Tools and Samples

Status: completed

Implementation Summary:

- Cleaned a narrow first wave of low-risk warnings in auxiliary code only:
  - `src/samplosf/src/dtsamples/dump_vdf.c`
  - `src/licunix/src/license.c`
  - `src/licunix/src/csn.c`
- Used internal linkage for private helper functions and explicit `(void)`
  markers for intentionally unused `main` parameters.
- Did not touch synthesis, phoneme, LTS, VTM, HLSYN, public API, dictionary
  source, audio, or threading paths.
- Verification run:
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase4-tools-samples --expected tests/golden`
  - `tools/baseline/summarize_warnings.py --log baseline-runs/phase4-tools-samples/build/build-strict-warnings.log --out-dir baseline-runs/phase4-tools-samples/warnings-strict`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated dictionary file list, sizes, and SHA-256 hashes matched committed
    baselines.
  - default warning-line count: 1,828.
  - strict warning-line count: 29,838.
  - parser-visible strict warnings: 29,818.
- No public headers, exported symbols, generated dictionaries, generated audio,
  install layout, or runtime behavior were intentionally changed.

Goals:

- Reduce warnings in command-line tools, samples, and auxiliary utilities.
- Avoid behavior-critical engine paths.
- Establish the cleanup pattern for future waves.

Allowed categories:

- missing includes
- missing prototypes in private `.c` files
- duplicate declarations
- unused parameters with explicit `(void)` markers
- obvious format-string fixes
- obvious local initialization where current behavior is unambiguous

Success criteria:

- Default and strict warning counts decrease.
- Golden audio still compares exactly.
- Exported symbols are unchanged.
- Dist manifest is unchanged unless a tool-only artifact change is explicitly
  documented.

Rules:

- No public header signature changes.
- No dictionary source changes.
- No audio/threading changes.

---

## Phase 5: Low-Risk Warning Cleanup, API Boundaries

Status: completed

Implementation Summary:

- Reviewed API-boundary warnings from the Phase 4 strict warning inventory.
- Added `docs/modernization/API_BOUNDARY_WARNINGS.md` with the API warning
  paths and warning types found under the API build context.
- Deferred API implementation cleanup because the remaining tracked API
  warnings are concentrated in high-risk or medium-risk areas:
  - `src/dapi/src/api/ttsapi.c`
  - `src/dapi/src/api/init.c`
  - `src/dapi/src/api/coop.h`
  - API-build-context objects for `services.c` and `usa_init.c`
- Verification run:
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
  - Phase 4 verification remains the current accepted behavior check:
    `tools/baseline/verify_current.sh --run-dir baseline-runs/phase4-tools-samples --expected tests/golden`
- Verification results:
  - no API source or public header files were changed.
  - no exported-symbol, public-header, audio, dictionary, or install-layout
    behavior changes were introduced by this documentation-only phase.
  - warning counts remain at the Phase 4 accepted baseline: default 1,828,
    strict 29,838.
- `src/dapi/src/api/ttsapi.h` and `src/dapi/src/api/tts.h` were intentionally
  left unchanged.

Goals:

- Reduce warnings near API implementation files while preserving ABI.
- Clean internal prototypes/includes where they do not alter public headers.
- Document any warning that appears low-risk but touches public API surfaces.

Candidate areas:

- private declarations in `src/dapi/src/api/*.c`
- private helper prototypes
- internal duplicate declarations
- sample/API boundary include hygiene

Success criteria:

- Exported symbols exactly match the accepted baseline.
- Installed public headers are byte-for-byte unchanged unless explicitly
  approved.
- Golden audio still compares exactly.
- Warning counts decrease or unchanged with clear explanation.

Rules:

- Do not change `ttsapi.h` or `tts.h` without explicit review.
- Do not change public API signatures or calling conventions.

---

## Phase 6: Dictionary Baseline Expansion

Status: completed

Implementation Summary:

- Added deterministic US user dictionary fixture input:
  - `tests/golden/dictionaries/user/input/us_custom_pronunciations.tab`
- Added committed user dictionary compiler expected captures:
  - `tests/golden/dictionaries/user/expected/files.txt`
  - `tests/golden/dictionaries/user/expected/sha256.txt`
  - `tests/golden/dictionaries/user/expected/sizes.tsv`
  - `tests/golden/dictionaries/user/expected/logs/us_custom_pronunciations.log`
- Added `tools/baseline/capture_user_dictionaries.sh` to compile fixture `.tab`
  files with `dist/tools/udic_us` and capture `.dtu` output metadata.
- Updated `tools/baseline/verify_current.sh` and Ubuntu CI to compare the user
  dictionary fixture output against the committed expected capture.
- Added `docs/modernization/DICTIONARY_BASELINE.md` and updated baseline
  documentation with the main/user dictionary capture workflow.
- Verification run:
  - `tools/baseline/capture_user_dictionaries.sh --out baseline-runs/phase6-user-dictionaries`
  - `tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries/user/expected --actual baseline-runs/phase6-user-dictionaries --out baseline-runs/phase6-user-dictionaries-compare`
  - `tools/baseline/capture_dictionaries.sh --out baseline-runs/phase6-main-dictionaries`
  - `tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/phase6-main-dictionaries --out baseline-runs/phase6-main-dictionaries-compare`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase6-dictionary-baseline --expected tests/golden`
- Verification results:
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated main dictionary file list, sizes, and SHA-256 hashes matched
    committed baselines.
  - generated user dictionary fixture file list, size, and SHA-256 hash matched
    committed baselines.
  - default warning-line count: 1,829.
  - strict warning-line count: 29,839.
- No dictionary source text, dictionary format, generated committed `.dic`
  files, public headers, exported symbols, generated audio, install layout, or
  runtime behavior were intentionally changed.

Goals:

- Make dictionary generation and lookup behavior safer to modernize.
- Capture generated dictionaries and dictionary tool behavior.
- Add focused checks around user dictionary tools.

Suggested work:

- Capture hashes for all generated `dist/dic/*.dic`.
- Capture dictionary compiler logs.
- Add fixed user dictionary input samples.
- Add compare scripts for dictionary outputs.
- Document dictionary source-to-output relationships.

Success criteria:

- Dictionary outputs can be regenerated and compared exactly.
- CI catches generated dictionary changes.
- No dictionary format or lookup behavior changes are made.

Rules:

- Do not normalize dictionary source text.
- Do not change generated `.dic` files except through an explicit approved
  baseline update.

---

## Phase 7: Golden Audio Expansion

Status: completed

Implementation Summary:

- Expanded golden audio verification reporting without adding non-US baselines.
  This preserves the previously approved scope: US English, speakers 0 through
  8, one fixed input, WAV output.
- Updated `tools/baseline/compare_audio.py` to optionally write a TSV metrics
  report with SHA-256 hashes, frame counts, sample rates, peak/RMS levels, and
  max sample deltas.
- Updated `tools/baseline/verify_current.sh` and Ubuntu CI to publish
  `audio-metrics.tsv` alongside exact audio comparison logs.
- Added documentation headers to `tools/baseline/capture_audio.sh` and updated
  `tools/baseline/README.md`, `tests/golden/audio/us/README.md`, and
  `docs/modernization/BASELINE_PROCEDURE.md`.
- Verification run:
  - `tools/baseline/capture_audio.sh --out baseline-runs/phase7-audio-us`
  - `tools/baseline/compare_audio.py --actual baseline-runs/phase7-audio-us --metrics-out baseline-runs/phase7-audio-us/audio-metrics.tsv`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase7-audio-verification --expected tests/golden`
- Verification results:
  - US English golden audio matched exactly for speakers 0 through 8.
  - audio metrics reported 56,729 frames, 11025 Hz, matching SHA-256 hashes,
    and max sample delta 0 for every committed speaker WAV.
  - all captured shared-library symbol lists matched committed baselines.
  - generated main and user dictionary file lists, sizes, and SHA-256 hashes
    matched committed baselines.
  - default warning-line count: 1,828.
  - strict warning-line count: 29,839.
- No generated WAV baselines, dictionary outputs, public headers, exported
  symbols, install layout, sample rate, or runtime behavior were intentionally
  changed.

Goals:

- Increase confidence before touching language, parser, phoneme, timing, or
  synthesis code.
- Cover more text shapes and language paths without relying on live audio
  hardware.

Suggested work:

- Add fixed text inputs for punctuation, numbers, abbreviations, commands, and
  common DECtalk markup.
- Add one or more non-US language golden sets after dictionary baselines are
  stable.
- Capture raw output or WAV output consistently.
- Add sample count, hash, peak, RMS, and max-delta reports.

Success criteria:

- Expanded golden set can be regenerated exactly on the supported Linux target.
- Comparison reports are concise enough for CI artifacts.
- CI runs the expanded set within acceptable time.

Rules:

- Do not use microphone/live audio hardware.
- Do not approve speech-output changes without explicit baseline update.

---

## Phase 8: CMake Parity, Core Linux Build

Status: completed

Implementation Summary:

- Expanded the optional side-by-side Linux CMake build in `CMakeLists.txt`:
  - added six language-library targets producing `libtts_us.so`,
    `libtts_uk.so`, `libtts_sp.so`, `libtts_gr.so`, `libtts_la.so`, and
    `libtts_fr.so`.
  - added the multi-language `tts_multi` target producing `libtts.so` from
    `src/dtalkml/src/dtalk_ml.c` and `src/dtalkml/src/init.c`.
  - added `dic_us`, `dic_uk`, `dic_sp`, `dic_gr`, `dic_la`, and `dic_fr`
    dictionary compiler targets plus generated dictionary outputs.
  - installed the generated dictionaries, CMake-built libraries, dictionary
    tools, selected public headers, `DECtalk.conf`, and `say` into the CMake
    staging tree.
  - linked the CMake `say` target through `libtts.so` so the same
    multi-language `-l us` path used by the Autotools sample is available.
- Updated `docs/modernization/CMAKE_OVERVIEW.md` with current CMake scope,
  source membership, dictionary generation notes, verification results, and
  remaining staged-layout gaps.
- Verification run:
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase8-cmake-expanded-2 -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase8-cmake-expanded-2 --target dectalk_cmake_stage -- -j1`
  - `tools/baseline/capture_dictionaries.sh --dist baseline-runs/phase8-cmake-expanded-2/cmake-dist --out baseline-runs/phase8-cmake-dictionaries`
  - `tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/phase8-cmake-dictionaries --out baseline-runs/phase8-cmake-dictionary-compare`
  - `tools/baseline/capture_symbols.sh --dist baseline-runs/phase8-cmake-expanded-2/cmake-dist --out baseline-runs/phase8-cmake-symbols`
  - `tools/baseline/compare_symbols.sh --expected tests/golden/symbols --actual baseline-runs/phase8-cmake-symbols --out baseline-runs/phase8-cmake-symbol-compare`
  - sorted symbol-name set comparison for all `libtts*.so` captures.
  - `tools/baseline/capture_audio.sh --dist baseline-runs/phase8-cmake-expanded-2/cmake-dist --out baseline-runs/phase8-cmake-audio-us`
  - `tools/baseline/compare_audio.py --actual baseline-runs/phase8-cmake-audio-us --metrics-out baseline-runs/phase8-cmake-audio-metrics.tsv`
  - `tools/baseline/capture_dist_manifest.sh --dist dist --out baseline-runs/phase8-autotools-manifest.txt`
  - `tools/baseline/capture_dist_manifest.sh --dist baseline-runs/phase8-cmake-expanded-2/cmake-dist --out baseline-runs/phase8-cmake-manifest.txt`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase8-autotools-manifest.txt --actual baseline-runs/phase8-cmake-manifest.txt --out baseline-runs/phase8-cmake-manifest.diff`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase8-behavior --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - CMake emitted `compile_commands.json`.
  - CMake staged build completed successfully.
  - CMake-generated dictionaries matched committed golden hashes exactly.
  - CMake-staged `say` produced exact US English golden WAVs for speakers 0
    through 8.
  - `libtts.so` matched the committed symbol capture exactly.
  - language-library full symbol captures differ by address and address-sorted
    order, but sorted symbol-name sets matched for all CMake-built language
    libraries.
  - CMake staged manifest is comparable but not yet exact: 541 Autotools dist
    entries are not yet staged by CMake, with no extra CMake entries.
  - Autotools behavior verification still passed; default warning-line count was
    1,829 and strict warning-line count was 29,839.
- No Autotools files, Visual Studio files, engine source, public API signatures,
  dictionary sources, committed generated dictionaries, exported symbol names,
  audio backend behavior, or threading behavior were intentionally changed.

Reasoning checkpoint: extra-high recommended before implementing source
membership, generated dictionary, staged install layout, or multi-language
library changes in this phase.

Goals:

- Bring side-by-side CMake closer to the Autotools Linux build.
- Keep CMake optional and non-authoritative until parity is proven.
- Preserve artifact names where compatibility matters.

Suggested work:

- Build all language-specific `libtts_<lang>.so` libraries.
- Build the multi-language `libtts.so` if source membership is understood.
- Build dictionary compiler targets.
- Generate/copy dictionary outputs into a CMake staging directory.
- Build minimal CLI tools needed for verification.
- Install/stage files into a CMake dist tree matching the Autotools layout.

Success criteria:

- CMake emits `compile_commands.json`.
- CMake-built source membership differences are documented and shrinking.
- CMake staged manifest can be compared to Autotools `dist`.
- CMake-built libraries export expected symbols, or differences are documented
  as known gaps.

Rules:

- Do not remove Autotools.
- Do not remove Visual Studio projects.
- Do not declare CMake parity until symbols, dictionaries, and staged layout are
  compared.

---

## Phase 9: Platform Abstraction, Time and Filesystem

Status: completed

Implementation Summary:

- Added internal POSIX filesystem wrapper scaffolding:
  - `src/platform/dt_filesystem.h`
  - `src/platform/dt_filesystem_posix.c`
- Expanded the existing internal time wrapper with
  `dt_monotonic_milliseconds()` in:
  - `src/platform/dt_time.h`
  - `src/platform/dt_time_posix.c`
- Added a CMake-only developer smoke tool,
  `tools/platform/dt_platform_smoke.c`, to exercise the time and filesystem
  wrappers without installing the tool or routing DECtalk runtime behavior
  through the wrappers.
- Updated `CMakeLists.txt` with a private `dt_platform` static library and
  `dt_platform_smoke` executable target.
- Updated `src/platform/README.md` to describe the current wrapper scope and the
  CMake-only smoke target.
- Verification run:
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase9-platform-cmake -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase9-platform-cmake --target dt_platform_smoke -- -j1`
  - `baseline-runs/phase9-platform-cmake/dt_platform_smoke . PLAN.md`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase9-behavior --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - CMake smoke target built and ran successfully.
  - Autotools behavior verification passed with exact golden audio, exported
    symbols, generated dictionaries, and user dictionaries.
  - default warning-line count was 1,829 and strict warning-line count was
    29,839.
- No engine source, public headers, dictionary sources, threading code, audio
  backend code, exported symbols, or runtime behavior paths were intentionally
  changed.

Goals:

- Start using platform wrappers in the lowest-risk areas.
- Keep thread and audio paths untouched until later phases.

Suggested work:

- Expand `src/platform/dt_time.*` only where behavior is simple and covered.
- Add `src/platform/dt_filesystem.*` for path helpers that wrap existing logic.
- Use wrappers first in tools or non-critical sample code.

Success criteria:

- Existing build succeeds.
- Golden audio and exported symbols are unchanged.
- No thread/audio behavior changes occur.

Rules:

- Do not replace `opthread.c`.
- Do not change `linux_audio.c`.
- Avoid public headers.

---

## Phase 10: Platform Abstraction, Threads and Synchronization

Status: completed

Implementation Summary:

- Added internal POSIX thread and synchronization wrapper scaffolding:
  - `src/platform/dt_thread.h`
  - `src/platform/dt_thread_posix.c`
  - `src/platform/dt_mutex.h`
  - `src/platform/dt_mutex_posix.c`
  - `src/platform/dt_event.h`
  - `src/platform/dt_event_posix.c`
- The new wrappers provide opaque internal handles for thread create/join/exit,
  mutex create/lock/unlock/destroy, and manual-reset/auto-reset event
  create/set/reset/wait/destroy.
- Extended the CMake-only `dt_platform` static library and
  `tools/platform/dt_platform_smoke.c` so the smoke target compiles and runs the
  thread, mutex, and event wrappers without installing them or routing existing
  runtime behavior through them.
- Updated `src/platform/README.md` to document the added wrapper scope.
- Verification run:
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase10-platform-cmake -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase10-platform-cmake --target dt_platform_smoke -- -j1`
  - `baseline-runs/phase10-platform-cmake/dt_platform_smoke . PLAN.md`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase10-behavior --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - CMake smoke target built and ran successfully, including thread create/join,
    mutex lock/unlock, and event wait/set behavior.
  - Autotools behavior verification passed with exact golden audio, exported
    symbols, generated dictionaries, and user dictionaries.
  - default warning-line count was 1,829 and strict warning-line count was
    29,837.
- No existing runtime thread lifecycle, `opthread.c`, `linux_audio.c`, callback,
  queue, pipe, audio, dictionary, public API, or exported-symbol behavior was
  intentionally changed.

Reasoning checkpoint: extra-high recommended before wiring any thread, mutex, or
event wrapper into existing runtime code.

Goals:

- Prepare thread/mutex/event wrappers without changing runtime behavior.
- Contain platform-specific threading APIs behind narrow interfaces.

Suggested work:

- Add `dt_thread.h`, `dt_mutex.h`, and `dt_event.h`.
- Implement POSIX wrappers by delegating to existing semantics.
- Add small standalone tests or compile checks for wrappers.
- Only wire wrappers into code after exact audio and API checks are stable.

Success criteria:

- Wrapper code compiles standalone.
- Existing thread/audio runtime paths remain unchanged unless explicitly approved
  in a later subtask.
- Behavior baselines still pass.

Rules:

- Do not rewrite thread lifecycle in one step.
- Do not change callback, queue, or pipe semantics.

---

## Phase 11: Audio Backend Containment

Status: completed

Implementation Summary:

- Added `docs/modernization/AUDIO_BACKEND.md` documenting the current
  Autotools audio options, backend selection order, `/dev/dsp` and `/dev/audio`
  legacy device behavior, ALSA `ALSA_DEFAULT` behavior, CMake audio-option gap,
  and deterministic no-hardware WAV verification policy.
- Added passive CMake-only audio metadata scaffolding:
  - `src/platform/dt_audio_backend.h`
  - `src/platform/dt_audio_backend.c`
- Updated `CMakeLists.txt` so `dt_platform` compiles the new metadata
  scaffolding.
- Updated `tools/platform/dt_platform_smoke.c` to print compiled audio backend
  metadata without opening audio devices or linking audio libraries.
- Updated `src/platform/README.md` and
  `docs/modernization/CMAKE_OVERVIEW.md` to describe the containment scope and
  current CMake audio limitations.
- Verification run:
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase11-audio-cmake -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase11-audio-cmake --target dt_platform_smoke -- -j1`
  - `baseline-runs/phase11-audio-cmake/dt_platform_smoke . PLAN.md`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase11-behavior --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - CMake smoke reported current Linux metadata with audio enabled, OSS
    available, no CMake ALSA/PulseAudio defines, and `/dev/dsp` as the legacy
    OSS device.
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated dictionary and user dictionary captures matched committed
    baselines.
  - default warning-line count: 1,829.
  - strict warning-line count: 29,839.
- `src/dapi/src/nt/linux_audio.c`, sample rate, callback timing, live backend
  selection, public headers, exported symbols, dictionaries, and runtime audio
  behavior were not changed.

Reasoning checkpoint: extra-high recommended before routing any existing audio
behavior through a new backend abstraction.

Goals:

- Isolate audio backend selection without changing default audio behavior.
- Make no-hardware WAV generation remain deterministic.

Suggested work:

- Document current `/dev/dsp`, ALSA, PulseAudio, and disabled-audio behavior.
- Add compile-time scaffolding for backend interfaces.
- Keep `linux_audio.c` intact until a wrapper can delegate exactly to existing
  behavior.

Success criteria:

- WAV output baselines remain exact.
- Live backend code paths are not altered unless explicitly approved.
- Configure/CMake audio options are documented.

Rules:

- Do not change sample rate.
- Do not change callback timing.
- Do not change default backend selection.

---

## Phase 12: Historical Target Quarantine, Source Level

Status: completed

Implementation Summary:

- Added a source-level historical target opt-in gate for platform scaffolding:
  - `src/platform/dt_legacy_targets.h`
  - `src/platform/dt_legacy_targets.c`
- Updated `src/platform/dt_platform.h` with standard project documentation.
- Updated `CMakeLists.txt` so the CMake platform smoke target compiles the
  historical target inventory and defines
  `DECTALK_ENABLE_LEGACY_TARGET_SOURCE` only when
  `DECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON` is set.
- Updated `src/configure.ac` and `src/config.h.in` so Autotools current Linux
  configuration remains default, while historical/non-current target triplets
  require `./configure --enable-legacy-targets`.
- Updated platform smoke output to report historical target macro inventory and
  the source opt-in state.
- Updated modernization docs:
  - `docs/modernization/HISTORICAL_TARGETS.md`
  - `docs/modernization/BUILD_OVERVIEW.md`
  - `docs/modernization/CMAKE_OVERVIEW.md`
  - `docs/modernization/MACRO_INVENTORY.md`
  - `src/platform/README.md`
- Verification run:
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase12-legacy-cmake -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase12-legacy-cmake --target dt_platform_smoke -- -j1`
  - `baseline-runs/phase12-legacy-cmake/dt_platform_smoke . PLAN.md`
  - `(cd src && autoreconf -i)`
  - `(cd src && ./configure --target=sparc-sun-solaris)`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase12-behavior --expected tests/golden`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - CMake platform smoke reported all historical target indicators inactive on
    the current Linux build and `legacy_source_opt_in=0`.
  - `./configure --target=sparc-sun-solaris` stopped with the new
    `--enable-legacy-targets` opt-in requirement.
  - Current Linux Autotools verification still passed.
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated dictionary and user dictionary captures matched committed
    baselines.
  - default warning-line count: 1,829.
  - strict warning-line count: 29,839.
- No historical code was deleted. No engine files, public headers, dictionary
  sources, runtime audio/threading paths, exported symbols, or Linux default
  runtime behavior were changed.

Goals:

- Reduce accidental interaction between Linux cleanup and historical branches.
- Move from documentation-only quarantine to explicit opt-in guards where safe.

Suggested work:

- Add legacy-target option names for Autotools/CMake where appropriate.
- Quarantine whole historical build branches before touching mixed source files.
- Document each quarantine with target, macro, files, and verification impact.

Success criteria:

- Linux default build is unchanged.
- Historical paths are still present but harder to activate accidentally.
- No historical code is deleted.

Rules:

- Inventory first.
- Quarantine second.
- Delete only with explicit approval.

---

## Phase 13: Medium-Risk Warning Cleanup

Status: completed

Implementation Summary:

- Addressed one focused warning category: dictionary compiler diagnostic
  format-string type mismatches in `src/dapi/src/dic/dic_comm.c`.
- Changed diagnostic-only formats so they match the values being printed:
  - `pid_t` from `getpid()` is printed through an explicit `long` cast with
    `%ld`.
  - `linenumber` uses `%ld`.
  - `header.no_of_entries` uses `%lu`.
- Focused verification strategy:
  - treat `dic_comm.c` as dictionary-generation sensitive.
  - require exact generated dictionary and user dictionary comparisons.
  - require exact golden audio and exported symbol comparisons after rebuild.
  - compare the captured dist manifest against the previous accepted Phase 12
    manifest.
- Verification run:
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase13-format --expected tests/golden`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase12-behavior/dist-manifest.txt --actual baseline-runs/phase13-format/dist-manifest.txt --out baseline-runs/phase13-format/dist-manifest-compare.diff`
  - `rg -n "dic_comm.c:.*warning: format" baseline-runs/phase13-format/build/build.log baseline-runs/phase13-format/build/build-strict-warnings.log`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - targeted `dic_comm.c` format warnings were eliminated.
  - default warning-line count decreased from 1,829 to 1,805.
  - strict warning-line count decreased from 29,839 to 29,815.
  - US English golden audio matched exactly for speakers 0 through 8.
  - all captured shared-library symbol lists matched committed baselines.
  - generated dictionary and user dictionary captures matched committed
    baselines.
  - dist manifest matched the Phase 12 manifest exactly.
- No dictionary write logic, dictionary binary structures, engine code, public
  headers, runtime audio/threading paths, exported symbols, or speech behavior
  were intentionally changed.

Reasoning checkpoint: extra-high recommended before changing conversions,
callback signatures, thread function signatures, `volatile`, or
concurrency-related code.

Goals:

- Address warnings that may affect types, conversions, callbacks, or sizes.
- Proceed only with expanded baselines in place.

Candidate categories:

- signed/unsigned conversions
- pointer/integer casts
- size truncation
- callback signature mismatches
- thread function signatures
- volatile/concurrency-related warnings

Success criteria:

- Each category has a focused verification strategy.
- Golden audio, dictionaries, symbols, and dist manifests remain unchanged unless
  an explicit behavior change is approved.
- Warning counts decrease by category.

Rules:

- Do not mix categories in one change.
- Stop on any unexplained output difference.

---

## Phase 14: Public Header and Export Audit

Status: completed

Implementation Summary:

- Added `docs/modernization/PUBLIC_API_AUDIT.md` documenting the current Linux
  installed header set, ABI-sensitive non-installed `tts.h`, feasible header
  isolation checks, installed phoneme headers that are intentionally not
  isolated, and exported-symbol policy.
- Added public header audit tooling:
  - `tools/baseline/check_public_headers.sh`
- Added committed public header allowlists:
  - `tests/golden/public-headers/installed-linux.txt`
  - `tests/golden/public-headers/self-compile-linux.txt`
  - `tests/golden/public-headers/known-not-isolated-linux.txt`
  - `tests/golden/public-headers/README.md`
- Updated `tools/baseline/verify_current.sh` so expected runs containing
  `public-headers/` also compare public header audit outputs.
- Updated `.github/workflows/build.yml` so the Ubuntu verification gate runs
  the public header audit against the committed allowlists.
- Updated `tools/baseline/README.md` with the public header audit command.
- Header self-compile coverage now includes:
  - `ttsapi.h`
  - `tts.h`
  - `dtmmedefs.h`
  - `l_com_ph.h`
  - `l_fr_ph.h`
- Verification run:
  - `tools/baseline/check_public_headers.sh --out baseline-runs/phase14-public-headers --expected tests/golden/public-headers`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase14-api --expected tests/golden`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase13-format/dist-manifest.txt --actual baseline-runs/phase14-api/dist-manifest.txt --out baseline-runs/phase14-api/dist-manifest-compare.diff`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - public header audit matched committed allowlists.
  - all feasible header self-compile checks passed.
  - exported symbol comparisons matched committed baselines for all shared
    libraries.
  - US English golden audio matched exactly for speakers 0 through 8.
  - generated dictionary and user dictionary captures matched committed
    baselines.
  - dist manifest matched the Phase 13 manifest exactly.
  - default warning-line count: 1,805.
  - strict warning-line count: 29,815.
- No public headers, public API signatures, exported symbol names, calling
  conventions, structure layouts, runtime code, dictionaries, or speech behavior
  were changed.

Reasoning checkpoint: extra-high recommended before changing public headers,
ABI surfaces, calling conventions, or exported-symbol policy.

Goals:

- Make public API boundaries explicit and testable.
- Prevent accidental ABI/API drift.

Suggested work:

- Document installed public headers.
- Add header self-compile checks.
- Add exported-symbol allowlists for public libraries.
- Add CI enforcement after allowlists are accepted.

Success criteria:

- Public headers compile in isolation where feasible.
- Export lists are versioned and compared in CI.
- No public API signature changes are made unless explicitly approved.

Rules:

- Treat public headers as high risk.
- Do not reformat public headers.

---

## Phase 15: Packaging and Install Layout Parity

Status: completed

Implementation Summary:

- Extended `tools/baseline/capture_dist_manifest.sh` with opt-in manifest
  formats while preserving the existing default output:
  - `basic`: path, type, and symlink target.
  - `metadata`: path, type, mode, size, and symlink target.
  - `metadata-hash`: metadata plus SHA-256 hashes for regular files.
- Added `docs/modernization/PACKAGING_LAYOUT.md` documenting the current
  Autotools `dist/` layout, generated `DECtalk.conf`, detailed manifest modes,
  and current CMake staging limitations.
- Updated `tools/baseline/README.md` with detailed packaging manifest commands.
- Verification run:
  - `tools/baseline/capture_dist_manifest.sh --out baseline-runs/phase15-default-dist-manifest.txt`
  - `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out baseline-runs/phase15-autotools-dist-manifest-detailed.txt`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase14-api/dist-manifest.txt --actual baseline-runs/phase15-default-dist-manifest.txt --out baseline-runs/phase15-default-manifest-compare.diff`
  - `CC=/usr/bin/gcc cmake -S . -B baseline-runs/phase15-cmake -DCMAKE_BUILD_TYPE=Release`
  - `cmake --build baseline-runs/phase15-cmake --target dectalk_cmake_stage -- -j1`
  - `tools/baseline/capture_dist_manifest.sh --dist baseline-runs/phase15-cmake/cmake-dist --out baseline-runs/phase15-cmake-dist-manifest.txt`
  - `tools/baseline/capture_dist_manifest.sh --dist baseline-runs/phase15-cmake/cmake-dist --format metadata-hash --out baseline-runs/phase15-cmake-dist-manifest-detailed.txt`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase15-autotools-dist-manifest-detailed.txt --actual baseline-runs/phase15-cmake-dist-manifest-detailed.txt --out baseline-runs/phase15-cmake-vs-autotools-detailed.diff`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase15-behavior --expected tests/golden`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase14-api/dist-manifest.txt --actual baseline-runs/phase15-behavior/dist-manifest.txt --out baseline-runs/phase15-behavior/dist-manifest-compare.diff`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - default path/type manifest output remained compatible and matched the Phase
    14 manifest exactly.
  - detailed Autotools manifest capture produced 1,126 lines.
  - detailed CMake staged manifest capture produced 90 lines.
  - CMake-vs-Autotools detailed manifest comparison intentionally differed,
    with 1,064 removed lines and 28 added lines in the diff, confirming CMake
    packaging remains a subset.
  - documented remaining CMake packaging gaps include docs, bitmaps, source
    sample trees, `/usr/bin` symlinks, `README`, and helper/sample tools such as
    `aclock`, `dtmemory`, `gspeak`, `windic`, `tunecheck`, user-dictionary
    tools, and demo tools.
  - US English golden audio matched exactly for speakers 0 through 8.
  - exported symbols, generated dictionaries, user dictionaries, public header
    audit, and default dist manifest comparisons all passed.
  - default warning-line count: 1,805.
  - strict warning-line count: 29,814.
- Existing install layout was not changed. No libraries, tools, headers,
  dictionaries, generated `DECtalk.conf` keys, symlinks, runtime code, or speech
  behavior were intentionally changed.

Goals:

- Make install/dist output reproducible and comparable.
- Prepare for CMake packaging parity without replacing Autotools.

Suggested work:

- Add manifest comparison with path, type, mode, symlink target, and optional
  hash modes.
- Compare Autotools dist against CMake staged output.
- Document generated config files such as `DECtalk.conf`.

Success criteria:

- Packaging changes are visible as manifest diffs.
- CMake staged layout differences are documented and intentionally reduced.
- Existing install layout is unchanged unless explicitly approved.

Rules:

- Do not rename libraries or tools.
- Do not move installed headers.

---

## Phase 16: Documentation Consolidation

Status: completed

Implementation Summary:

- Added `docs/modernization/README.md` as the modern maintenance index linking
  build overview, baseline procedure, risk areas, warning and macro inventory,
  public API audit, packaging layout, dictionary baseline, CMake status,
  historical targets, audio backend notes, and API-boundary warning notes.
- Updated `README.md` with a concise current-modernization section that:
  - states Linux is the current modernization and cleanup target.
  - clarifies historical platforms are preserved but not current cleanup
    targets unless a phase explicitly says so.
  - links to `docs/modernization/README.md`.
  - shows the main Linux verification gate:
    `tools/baseline/verify_current.sh --run-dir baseline-runs/current --expected tests/golden`.
- Verification run:
  - checked that every documentation file linked from the modernization index
    exists.
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- No legacy documentation was deleted or rewritten wholesale. No build scripts,
  source files, public headers, runtime behavior, symbols, dictionaries, or
  packaging layout were changed.

Goals:

- Make modern build, verification, and cleanup workflows discoverable.
- Keep historical documentation intact while adding modern entry points.

Suggested work:

- Update `README.md` with current Linux build and verification workflow.
- Add a modernization index in `docs/modernization/README.md`.
- Link baseline scripts, CMake status, warning inventory, and historical target
  docs.
- Document what is supported now versus preserved historically.

Success criteria:

- A new contributor can build, run baseline checks, and understand supported
  targets from docs.
- Historical documentation is not deleted.

Rules:

- Do not rewrite legacy docs wholesale.

---

## Phase 17: Warning Policy Ratchet

Status: completed

Implementation Summary:

- Added narrow warning budget tooling:
  - `tools/baseline/check_warning_budgets.py`
- Added committed warning budget documentation and baseline:
  - `tests/golden/warnings/README.md`
  - `tests/golden/warnings/default-cleaned.tsv`
- Established the first cleaned-category budget:
  - `src/dapi/src/dic/dic_comm.c`
  - `-Wformat=`
  - maximum count `0`
- Updated `.github/workflows/build.yml` so Ubuntu CI summarizes default build
  warnings and fails only if committed cleaned-category budgets regress.
- Updated `tools/baseline/verify_current.sh` so local verification summarizes
  default warnings and checks accepted warning budgets when present under the
  expected directory.
- Updated `tools/baseline/README.md` and
  `docs/modernization/WARNING_INVENTORY.md` with the warning-budget ratchet
  workflow and policy.
- Verification run:
  - `tools/baseline/summarize_warnings.py --log baseline-runs/phase15-behavior/build/build.log --out-dir baseline-runs/phase17-warnings-default`
  - `tools/baseline/check_warning_budgets.py --warnings baseline-runs/phase17-warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/phase17-warning-budget.tsv`
  - `tools/baseline/verify_current.sh --run-dir baseline-runs/phase17-policy --expected tests/golden`
  - `tools/baseline/compare_manifest.sh --expected baseline-runs/phase15-behavior/dist-manifest.txt --actual baseline-runs/phase17-policy/dist-manifest.txt --out baseline-runs/phase17-policy/dist-manifest-compare.diff`
  - `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
- Verification results:
  - warning budget status was `ok`.
  - `dic_comm.c` `-Wformat=` actual count was `0` against budget `0`.
  - default warning-line count: 1,805.
  - strict warning-line count: 29,815.
  - US English golden audio matched exactly for speakers 0 through 8.
  - exported symbols, generated dictionaries, user dictionaries, public header
    audit, and dist manifest comparisons all passed.
- No compiler-wide warning fatal policy was added. No broad suppressions were
  introduced. Existing legacy warning debt remains tracked but non-blocking
  unless it is covered by an explicit cleaned-category budget.

Goals:

- Move warning cleanup from ad hoc progress to enforceable policy.
- Avoid blocking unrelated work on legacy warning debt too early.

Suggested work:

- Establish per-category warning budgets.
- Fail CI only on new warnings in cleaned categories.
- Keep total-warning trend reports as artifacts.
- Later, make specific low-risk categories fatal.

Success criteria:

- CI prevents warning regressions in cleaned files/categories.
- Existing warning debt remains tracked but does not block unrelated changes.

Rules:

- Do not make all warnings fatal.
- Do not introduce broad suppressions to hide warnings.

---

## Phase 18: CMake Promotion Decision

Status: not started

Reasoning checkpoint: extra-high recommended before making the promotion
recommendation.

Goals:

- Decide whether CMake is ready to become a primary Linux build path.
- Keep old build systems until parity is proven and approved.

Required evidence:

- CMake builds all intended Linux artifacts.
- CMake staged manifest matches Autotools or differences are approved.
- Symbols match or differences are approved.
- Generated dictionaries match.
- Golden audio matches.
- CI runs CMake checks.
- `compile_commands.json` works for analysis tooling.

Success criteria:

- A written recommendation is added to `docs/modernization/CMAKE_OVERVIEW.md`.
- No build system is removed in this phase.

Rules:

- Promotion is a decision point, not automatic removal of Autotools.

---

## Phase 19: Final Modernization Readiness Review

Status: not started

Reasoning checkpoint: extra-high recommended before final readiness conclusions
and before recommending any higher-risk follow-on objectives.

Goals:

- Confirm modernization goals are met without hidden behavior changes.
- Produce a concise state-of-the-codebase report.

Required review areas:

- Linux build reproducibility.
- CI reliability.
- warning counts and policy.
- public headers and exports.
- dictionary generation.
- golden audio coverage.
- CMake status.
- platform abstraction status.
- historical target quarantine status.
- known risks and deferred work.

Success criteria:

- `docs/modernization/READINESS_REVIEW.md` exists.
- All accepted baselines are reproducible.
- Known limitations are explicit.
- No behavior preservation claim is made beyond checks actually run.

---

## Definition of Done for Each Phase

A phase is not complete unless its Implementation Summary states:

- what changed
- why it changed
- files changed
- verification commands run
- whether warning counts changed
- whether public exports changed
- whether dictionaries changed
- whether golden audio changed
- behavior risk level
- known limitations

## Current Immediate Next Step

Start Phase 1: Verification Hardening.
