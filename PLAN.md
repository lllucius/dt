# PLAN.md

## Objective

Continue DECtalk modernization after the completed baseline-modernization plan,
while preserving current Linux behavior.

This plan moves into higher-risk objectives only after strengthening the
verification net. The goal is to reduce remaining warning debt, improve build
parity, and prepare isolated runtime cleanup without accidental changes to
speech output, public API behavior, generated dictionaries, packaging, or
runtime behavior.

## Guiding Principle

Prove the behavior boundary before changing code inside it.

For this phase of modernization, cleanup is subordinate to reproducibility.
Every risky change must be preceded by a baseline that can detect the relevant
class of regression.

## Existing Knowledge Base

The previous plan is complete and summarized by:

- `docs/modernization/READINESS_REVIEW.md`
- `docs/modernization/README.md`
- `docs/modernization/BASELINE_PROCEDURE.md`
- `docs/modernization/BUILD_OVERVIEW.md`
- `docs/modernization/RISK_AREAS.md`
- `docs/modernization/WARNING_INVENTORY.md`
- `docs/modernization/MACRO_INVENTORY.md`
- `docs/modernization/PUBLIC_API_AUDIT.md`
- `docs/modernization/DICTIONARY_BASELINE.md`
- `docs/modernization/PACKAGING_LAYOUT.md`
- `docs/modernization/CMAKE_OVERVIEW.md`
- `docs/modernization/HISTORICAL_TARGETS.md`
- `docs/modernization/AUDIO_BACKEND.md`
- `tools/baseline/`
- `tests/golden/`
- `src/platform/`

Known Phase 19 readiness facts:

- Autotools remains the authoritative Linux build path.
- `tools/baseline/verify_current.sh --run-dir baseline-runs/phase19-readiness --expected tests/golden`
  passed.
- Default warning-line count: 1,805.
- Strict warning-line count: 29,815.
- Parser-visible default warnings: 1,784.
- US English golden WAVs for speakers 0 through 8 matched exactly.
- Exported symbols matched the committed symbol baselines.
- Main dictionaries and the US user-dictionary fixture matched committed
  baselines.
- Public header allowlists matched.
- The initial warning budget for `src/dapi/src/dic/dic_comm.c` `-Wformat=`
  remained at zero.
- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/phase19-readiness-cmake --expected tests/golden`
  passed.
- CMake is still side-by-side only. It is not ready to replace Autotools.
- CMake staged packaging remains a subset of Autotools packaging.
- Live audio hardware behavior and non-current platform builds were not
  verified.
- Existing high-risk warning debt remains in synthesis, phoneme, LTS, VTM,
  HLSYN, public API, threading, and audio code.

## Operating Rules

- Work phases strictly in order.
- Do not advance while the current phase has failing checks or unresolved
  regressions.
- Keep each subtask small enough to leave the tree buildable and testable.
- Update this plan immediately after each completed phase with an
  Implementation Summary.
- Commit often while executing the plan. Each logical subtask should end in a
  stable, verified commit when feasible.
- Automatically add intended files to git and commit completed checkpoints.
- After completing the requested plan scope, create a pull request with an
  appropriate body summarizing changes, verification, behavior risk, and
  limitations, then automatically merge that PR once required checks and
  repository policy allow it.
- Do not delete historical target code without explicit approval.
- Do not change speech output, phoneme output, dictionary behavior, public API
  signatures, exported symbols, install layout, sample rate, default voice,
  threading model, or audio backend behavior unless a phase explicitly approves
  that change and verification proves the result.
- Every newly created source or header file must include standard project
  documentation at the top of the file: purpose, scope, behavior-preservation
  notes, and important limitations or ownership boundaries. Keep comments
  concise and avoid changing existing file headers unless the phase explicitly
  requires it.
- Before executing this plan, inspect the current tree and ask all upfront
  clarification questions in one response if any phase requirement is ambiguous.
- Execute ordinary phases with high reasoning.
- At each reasoning escalation checkpoint, stop before implementation, call out
  the need for extra-high reasoning, and wait for explicit approval before
  continuing that phase.
- If consecutive phases use the same reasoning level, continue without asking
  again unless a new ambiguity or regression appears.
- When extra-high checkpoint work is complete and the next work can return to
  ordinary high reasoning, stop before continuing, call out that the reasoning
  level should change back to high, and wait for explicit approval.

## Reasoning Escalation Checkpoints

Extra-high reasoning is required before:

- accepting or updating any golden behavior baseline
- adding non-US or phoneme/output baselines that may expose previously
  untracked differences
- changing CMake source membership, generated dictionary flow, staged packaging
  layout, or multi-language `libtts.so` behavior
- changing public headers, public API signatures, calling conventions, exported
  symbols, installed header layout, or ABI-sensitive structures
- changing API implementation near `ttsapi.c`, `tts.h`, `ttsapi.h`, `init.c`,
  callback paths, or multi-language loader behavior
- changing medium-risk warnings involving conversions, pointer qualifiers,
  pointer/integer casts, callback signatures, thread function signatures,
  `volatile`, structure layout, or size truncation
- wiring `src/platform` thread, mutex, event, filesystem, time, or audio
  wrappers into existing runtime code
- changing `src/dapi/src/nt/opthread.c` or `src/dapi/src/nt/linux_audio.c`
- simplifying sound-critical or language-selection macros
- quarantining historical target source branches beyond build-system opt-in
- promoting CMake to a primary Linux build path

If any phase finds an unexplained audio, dictionary, symbol, manifest, public
header, API, phoneme, or timing delta, treat the investigation as an extra-high
reasoning checkpoint.

## Standard Verification Gate

Every phase that changes code or build behavior must run the narrowest relevant
checks and state the result.

Default gate:

```sh
tools/baseline/verify_current.sh --run-dir baseline-runs/<phase> --expected tests/golden
git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'
```

Use additional checks when relevant:

- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/<phase>-cmake --expected tests/golden`
- `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out baseline-runs/<phase>/dist-manifest-detailed.txt`
- `tools/baseline/check_public_headers.sh --out baseline-runs/<phase>/public-headers --expected tests/golden/public-headers`
- `tools/baseline/summarize_warnings.py --log baseline-runs/<phase>/build/build.log --out-dir baseline-runs/<phase>/warnings-default`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/<phase>/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/<phase>/warning-budget.tsv`

Do not claim behavior preservation unless the relevant checks were run.

## Phase 1: Post-Merge Baseline Refresh

Status: completed

Reasoning checkpoint: high.

Goals:

- Re-sync the local working tree to the merged modernization PR.
- Re-run the current accepted Linux baseline from the merged branch.
- Record the post-merge baseline as the starting point for this plan.

Required work:

- Ensure local `develop` tracks the merged remote `develop` branch.
- Run the Autotools verification gate against `tests/golden`.
- Run the CMake subset verification gate.
- Capture current warning counts and compare them to Phase 19 readiness values.
- Document the refreshed run in `docs/modernization/READINESS_REVIEW.md` or a
  new follow-on note.

Success criteria:

- Local tree is clean before modifications begin.
- Autotools accepted baselines reproduce.
- CMake subset baselines reproduce.
- Warning counts are recorded.
- No source behavior changes are made.

Rules:

- This phase is a synchronization and verification checkpoint only.
- Do not update golden files in this phase.

Implementation Summary:

- Re-synced local `develop` with merged `origin/develop` by fetching the remote
  merge commit, rebasing the new plan commit onto `origin/develop`, and setting
  local `develop` to track `origin/develop`.
- Updated `docs/modernization/READINESS_REVIEW.md` with the follow-on Phase 1
  post-merge baseline refresh results.
- Verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next-phase1-post-merge --expected tests/golden`,
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next-phase1-post-merge-cmake --expected tests/golden`,
  and `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Verification results: Autotools accepted baselines reproduced for US English
  golden audio, exported symbols, main dictionaries, the US user-dictionary
  fixture, public header allowlists, and the narrow warning budget. CMake
  subset verification reproduced dictionaries, US English golden audio, exact
  `libtts.so` symbols, language-library symbol name/type sets, and
  `compile_commands.json`.
- Warning counts changed only by observation after the post-merge refresh:
  default warning-line count was 1,804, strict warning-line count was 29,815,
  and parser-visible default warnings were 1,783. The warning budget remained
  `ok`.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change. Behavior risk is low for this documentation and verification
  phase.
- Known limitations remain: CMake packaging is still a subset, live audio
  hardware behavior was not tested, and non-current platform builds were not
  verified.

## Phase 2: Baseline Coverage Expansion Plan

Status: completed

Reasoning checkpoint: high for inventory; extra-high before accepting new
golden outputs.

Goals:

- Decide which additional deterministic behavior baselines are worth adding
  before higher-risk cleanup.
- Prefer no-hardware checks.

Candidate coverage:

- Additional US English fixed text inputs already committed under
  `tests/golden/input/`.
- Optional non-US language WAV baselines.
- Optional phoneme or text-mode output captures if stable command paths exist.
- Optional API smoke tests for public open/close/speak-to-file behavior.
- Optional detailed packaging manifest baseline under `tests/golden/`.

Required work:

- Inventory available command-line options and test paths without changing
  runtime code.
- Document which baselines are stable, useful, and cheap enough for CI.
- Identify which baselines require explicit approval before committing outputs.

Success criteria:

- A written recommendation identifies the next baselines to add.
- No golden outputs are accepted without extra-high approval.
- No behavior-changing source edits are made.

Rules:

- Do not broaden CI runtime substantially without documenting cost.
- Do not add flaky live-audio or hardware-dependent checks.

Implementation Summary:

- Added `docs/modernization/BASELINE_EXPANSION_PLAN.md` with the Phase 2
  inventory findings and recommendations for deterministic baseline expansion.
- Updated `docs/modernization/README.md` to link the baseline expansion plan.
- Inventory commands reviewed `dist/say -h`, existing audio capture/comparison
  scripts, committed input files, public header audit tooling, and temporary
  WAV probes under `baseline-runs/next-phase2-inventory/`.
- Verification run: `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Verification results: docs-only phase; no source, build, dictionary, symbol,
  public-header, or golden audio files changed. Temporary probes showed the
  additional committed US inputs and installed non-US language paths can produce
  RIFF/WAVE PCM, 16-bit, mono, 11025 Hz files.
- Warning counts were not rerun in this docs-only phase; Phase 1 counts remain
  the current baseline. Public exports did not change. Dictionaries did not
  change. Golden audio did not change. Behavior risk is low.
- Known limitations: no new golden outputs were accepted; non-US baselines need
  language-specific input selection; phoneme/text output baselines need a
  stable textual capture path before they are recommended for CI.

## Phase 3: Deterministic Baseline Expansion

Status: completed

Reasoning checkpoint: extra-high before accepting new golden outputs.

Goals:

- Add the approved deterministic baselines from Phase 2.
- Make the new baselines reproducible through `tools/baseline/`.

Required work:

- Add or extend capture and comparison scripts for the approved baseline types.
- Commit only generated outputs that are intentionally accepted as golden
  baselines.
- Update CI to run the added baseline checks when runtime cost is reasonable.
- Document exact generation commands and limitations.

Success criteria:

- New baselines reproduce exactly on the current Linux target.
- Existing Phase 19 baselines still reproduce.
- CI runs the relevant new checks or explicitly documents why they remain local.

Rules:

- Any golden output delta is a behavior decision and must be explicitly
  approved before commit.
- Do not use live audio hardware.

Implementation Summary:

- Added expanded US English audio suite tooling:
  - `tools/baseline/capture_audio_suites.sh`
  - `tools/baseline/compare_audio_suites.sh`
- Updated `tools/baseline/verify_current.sh`,
  `tools/baseline/verify_cmake_subset.sh`, and `.github/workflows/build.yml`
  so local verification and Ubuntu CI capture and compare the expanded audio
  suites.
- Added accepted expanded US English WAV baselines:
  - `tests/golden/audio/us/us_abbreviations/speaker_0.wav` through
    `speaker_8.wav`
  - `tests/golden/audio/us/us_commands_markup/speaker_0.wav` through
    `speaker_8.wav`
  - `tests/golden/audio/us/us_punctuation_numbers/speaker_0.wav` through
    `speaker_8.wav`
- Added accepted detailed Autotools packaging manifest baseline:
  - `tests/golden/dist-manifest-detailed.txt`
- Updated baseline documentation in `tools/baseline/README.md`,
  `docs/modernization/BASELINE_PROCEDURE.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/BASELINE_EXPANSION_PLAN.md`, and
  `tests/golden/audio/us/README.md`.
- Verification run:
  `tools/baseline/capture_audio_suites.sh --out baseline-runs/next-phase3-audio-suites`,
  `tools/baseline/compare_audio_suites.sh --actual baseline-runs/next-phase3-audio-suites --metrics-out baseline-runs/next-phase3-audio-suite-metrics`,
  `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out baseline-runs/next-phase3-dist-manifest-detailed.txt`,
  `tools/baseline/compare_manifest.sh --expected tests/golden/dist-manifest-detailed.txt --actual baseline-runs/next-phase3-dist-manifest-detailed.txt --out baseline-runs/next-phase3-dist-manifest-detailed.diff`,
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next-phase3-verify --expected tests/golden`,
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next-phase3-cmake --expected tests/golden`,
  and `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Verification results: all 27 expanded US WAV files compared exactly; the
  detailed Autotools manifest compared exactly and contains 1,126 lines;
  Autotools accepted baselines reproduced for original US golden audio,
  exported symbols, main dictionaries, the US user-dictionary fixture, public
  header allowlists, warning budget, expanded US audio suites, and detailed
  manifest. CMake subset verification reproduced dictionaries, original US
  audio, expanded US audio suites, exact `libtts.so` symbols,
  language-library symbol name/type sets, and `compile_commands.json`.
- Warning counts did not improve in this phase: the full Autotools run observed
  1,805 default warning lines and 29,815 strict warning lines. The warning
  budget remained `ok`.
- Public exports did not change. Dictionaries did not change. Existing golden
  audio did not change; new golden audio suites were intentionally accepted
  under the extra-high checkpoint. Behavior risk is low because runtime code was
  not changed.
- Known limitations: expanded WAV baselines remain US English only; non-US
  language baselines and phoneme/text baselines remain deferred; live audio
  hardware behavior was not tested.

## Phase 4: CMake Packaging Parity, Non-Promoting

Status: not started

Reasoning checkpoint: extra-high before changing staged layout or install
semantics.

Goals:

- Reduce the CMake staged packaging gap while keeping Autotools authoritative.
- Make CMake useful for analysis and Linux verification without promotion.

Required work:

- Compare detailed Autotools and CMake staged manifests.
- Add missing low-risk staged assets where the source of truth is obvious.
- Preserve library names, tool names, dictionary layout, and `DECtalk.conf`
  semantics.
- Keep `tools/baseline/verify_cmake_subset.sh` authoritative for CMake subset
  checks.

Success criteria:

- CMake staged manifest gap is reduced and documented.
- CMake dictionaries, US English WAVs, and symbols still match accepted
  baselines.
- Autotools accepted baselines still pass.
- CMake is not promoted.

Rules:

- Do not remove Autotools, Visual Studio files, legacy project files, or
  existing packaging paths.
- Do not change runtime install layout unless explicitly approved.

## Phase 5: Warning Budget Expansion, Low-Risk Files

Status: not started

Reasoning checkpoint: high for low-risk auxiliary files; extra-high if cleanup
touches API, dictionary internals, synthesis, threading, or audio paths.

Goals:

- Expand warning-budget enforcement one cleaned category at a time.
- Avoid broad warning suppressions.

Preferred targets:

- auxiliary tools
- samples
- private command utilities
- already-cleaned categories with low behavior risk

Required work:

- Use `tools/baseline/summarize_warnings.py` to identify candidate warnings.
- Clean one narrow category/file group per checkpoint.
- Add matching entries to `tests/golden/warnings/default-cleaned.tsv`.
- Keep warnings non-fatal outside cleaned budgets.

Success criteria:

- Default or strict warning counts decrease or a cleaned category becomes
  budgeted.
- Autotools accepted baselines pass.
- No public exports, dictionaries, or golden audio change.

Rules:

- Do not mix unrelated warning categories.
- Do not silence warnings globally to make budgets pass.

## Phase 6: Public API Smoke Coverage

Status: not started

Reasoning checkpoint: extra-high before adding API tests or changing any API
implementation.

Goals:

- Add small public API behavior coverage before API-boundary cleanup.
- Keep public headers and exported symbols stable.

Required work:

- Identify a minimal C smoke test that includes installed public headers and
  exercises open, speak-to-file, and close behavior without live audio.
- Build the smoke test against the staged Linux install.
- Compare generated output with accepted deterministic baselines where
  practical.
- Add CI coverage if stable.

Success criteria:

- Public API smoke test compiles and runs on Linux.
- Existing public header and exported-symbol checks still pass.
- No public API signature or ABI change is made.

Rules:

- Do not reformat public headers.
- Do not change installed header paths.
- Do not change exported symbol names.

## Phase 7: API-Boundary Warning Cleanup

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Reduce a narrow set of API-adjacent warnings after smoke coverage exists.
- Preserve public API and ABI.

Allowed candidates:

- missing private prototypes in implementation files
- local unused parameters with explicit `(void)` markers
- missing standard includes where the declaration is unambiguous
- local initialization fixes where behavior is clear and verified

Required work:

- Select one warning category and one ownership area.
- Prove public headers and exported symbols are unchanged.
- Run API smoke coverage and accepted behavior baselines.
- Add warning budget entries only for cleaned categories.

Success criteria:

- Target warning category is reduced.
- API smoke test passes.
- Public headers and exported symbols are unchanged.
- Dictionaries and golden audio are unchanged.

Rules:

- Do not change public signatures, structure layout, calling conventions, or
  callback contracts.
- Do not clean pointer/integer or callback warnings casually.

## Phase 8: Platform Wrapper Wiring Decision

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Decide whether any `src/platform` wrappers are ready to be wired into runtime
  code.
- Prefer decision documentation over implementation unless verification is
  strong enough.

Required work:

- Compare existing runtime ownership in `opthread.c`, `linux_audio.c`, and
  related API paths with the wrapper scaffolding.
- Identify the smallest possible wrapper wiring candidate, if any.
- Define the exact behavior checks needed before and after wiring.
- Recommend proceed, defer, or add more tests.

Success criteria:

- Written decision is added to modernization docs.
- No runtime wrapper wiring happens unless this phase explicitly approves it
  and lists the verification gates.

Rules:

- Do not change thread lifecycle, queue behavior, pipe behavior, callback
  timing, sample rate, or audio backend selection in this decision phase.

## Phase 9: Runtime Wrapper Pilot

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- If Phase 8 approves a candidate, wire exactly one low-risk wrapper path.
- Preserve runtime behavior.

Required work:

- Implement only the approved wrapper integration.
- Keep the change mechanically small and easy to revert.
- Run all behavior gates identified in Phase 8.
- Capture before/after evidence.

Success criteria:

- Approved wrapper pilot passes all required checks.
- No audio, dictionary, symbol, public header, or packaging regression occurs.
- If a delta appears, stop and either fix it or revert the pilot.

Rules:

- Do not combine wrapper wiring with warning cleanup.
- Do not touch live audio backend routing unless explicitly approved.

## Phase 10: CMake Promotion Reassessment

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Reassess whether CMake is closer to primary Linux build readiness.
- Keep the decision separate from implementation.

Required evidence:

- CMake builds all intended Linux artifacts.
- CMake staged manifest matches Autotools or every difference is approved.
- Symbols match or differences are explicitly accepted as non-ABI-relevant.
- Generated dictionaries match.
- Golden audio and any expanded baselines match.
- CI runs CMake checks.
- `compile_commands.json` works for analysis tooling.

Success criteria:

- `docs/modernization/CMAKE_OVERVIEW.md` contains an updated recommendation.
- No build system is removed in this phase.

Rules:

- Promotion is a decision point, not automatic removal of Autotools.

## Phase 11: Final Readiness Review For This Plan

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Confirm the higher-risk follow-on plan completed without hidden behavior
  changes.
- Produce a concise state-of-the-codebase report.

Required review areas:

- Linux build reproducibility.
- CI reliability.
- warning counts and budgets.
- public headers, API smoke coverage, and exports.
- dictionary generation.
- golden audio and any expanded deterministic baselines.
- CMake status.
- platform abstraction status.
- historical target quarantine status.
- known risks and deferred work.

Success criteria:

- `docs/modernization/READINESS_REVIEW.md` is updated or a new dated follow-on
  readiness review exists.
- All accepted baselines are reproducible.
- Known limitations are explicit.
- No behavior preservation claim is made beyond checks actually run.

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

Before implementing Phase 1, review this plan against the current codebase and
ask any upfront clarification questions. If there are no blockers, start Phase
1 with high reasoning.
