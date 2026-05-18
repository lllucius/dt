# PLAN.md

## Objective

Continue DECtalk modernization after PR #4 while preserving current Linux
behavior.

This plan deliberately groups all ordinary high-reasoning work first, followed
by one uninterrupted extra-high block for the higher-risk objectives. The goal
is to reduce monitoring burden during execution while still keeping risky
changes behind explicit verification gates.

The plan advances documentation, verification, warning-budget ratchets, public
API smoke coverage, legacy platform-wrapper evidence, and CMake parity
evidence. It does not approve speech output changes, phoneme output changes,
dictionary format changes, public API changes, install-layout changes,
live-audio behavior changes, threading behavior changes, CMake promotion, or
historical target deletion.

## Guiding Principle

Behavior preservation is more important than cleanup.

Every risky change must have a matching verification gate before
implementation. If a gate cannot detect the relevant class of regression, stop
and improve the gate or defer the change.

## Current Intended Target

The current supported target for modernization work is Linux.

Historical targets such as Windows, macOS, IOS, OSF/Tru64, VxWorks, MS-DOS,
Windows CE, Solaris/SPARC, ARM7, MIPS, old PowerPC Mac, iPAQ Linux, and
Emscripten may be inventoried, documented, or kept behind explicit legacy
options. They must not be deleted or broadly rewritten without explicit
approval. macOS is a non-current target.

## Existing Knowledge Base

Use the existing modernization knowledge base before making changes:

- `docs/modernization/READINESS_REVIEW.md`
- `docs/modernization/README.md`
- `docs/modernization/BASELINE_PROCEDURE.md`
- `docs/modernization/BASELINE_EXPANSION_PLAN.md`
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
- `docs/modernization/PLATFORM_WRAPPER_DECISION.md`
- `docs/modernization/PHONEME_BASELINE_FEASIBILITY.md`
- `docs/modernization/API_BOUNDARY_WARNINGS.md`
- `docs/modernization/NEXT_PLAN_GATE_MAP.md`
- `tools/baseline/`
- `tests/golden/`
- `src/platform/`

Current starting facts after PR #4:

- PR #4, `Complete current modernization plan`, was merged into `develop` as
  merge commit `0555a8fef13d39f7e31ac96bd582ffb05f34db98`.
- Local `develop` was synchronized with `origin/develop` after the merge.
- Autotools remains the authoritative Linux build path.
- The final Autotools gate for the previous plan passed:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next2-phase12-readiness --expected tests/golden`.
- Previous final warning counts were: default warning-line count `1,778`,
  strict warning-line count `29,764`, and parser-visible default warnings
  `1,757`.
- The warning budget passed and currently ratchets cleaned warning classes for:
  `src/dapi/src/dic/dic_comm.c`, `src/samplosf/src/dtsamples/mfg_load.c`,
  `src/licunix/src/liceninc.c`, and `src/dapi/src/api/ttsapi.c`.
- Public headers, exported symbols, generated dictionaries, the US
  user-dictionary fixture, the public API smoke WAV, one-shot US English WAVs,
  and expanded deterministic US English audio suites matched accepted
  baselines.
- The final CMake subset gate for the previous plan passed:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next2-phase12-readiness-cmake --expected tests/golden`.
- CMake basic staged manifest path/type parity matched Autotools exactly:
  589 Autotools entries and 589 CMake entries.
- CMake detailed metadata-hash parity is not accepted: CMake-built binary
  metadata/hashes and one documentation directory metadata entry still differ.
- CMake does not yet expose the Autotools live-audio backend option surface.
- CMake remains side-by-side and must not be promoted by this plan.
- `src/platform/` remains isolated scaffolding. Runtime wrapper wiring is still
  deferred because legacy `OP_*`, audio, callback, queue, and timing semantics
  are not proven.
- No deterministic phoneme or text-mode golden baseline was accepted in the
  previous plan.
- Live audio hardware behavior, callback timing, queue behavior, backend device
  selection, non-current platform builds, non-US audio, and phoneme/text output
  remain outside verified behavior.
- High-risk warning debt remains in synthesis, phoneme, LTS, VTM, HLSYN,
  public API, threading, and audio code.

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
  Do not ask for confirmation before ordinary `git add` or `git commit`
  commands.
- After completing the requested plan scope, create a pull request with an
  appropriate body summarizing changes, verification, behavior risk, and
  limitations, then automatically merge that pull request once required checks
  and repository policy allow it. If repository auto-merge is unavailable,
  manually merge when GitHub accepts the merge.
- Use single-threaded builds where a command exposes parallelism. Do not choose
  `-j` values greater than `1` while executing this plan.
- Do not delete historical target code without explicit approval.
- Do not intentionally change speech output, phoneme output, dictionary
  behavior, public API signatures, exported symbols, install layout, command
  line behavior, sample rate, default voice, threading model, or audio backend
  behavior unless a phase explicitly approves that change and verification
  proves the result.
- Do not simplify sound-critical, language-selection, product-feature, or
  historical-target macros without classifying them first.
- Every newly created source or header file must include standard project
  documentation at the top of the file: purpose, scope, behavior-preservation
  notes, and important limitations or ownership boundaries. Keep comments
  concise and avoid changing existing file headers unless the phase explicitly
  requires it.
- Before executing this plan, inspect the current tree and ask all upfront
  clarification questions in one response if any phase requirement is
  ambiguous.
- If a phase finds an unexplained audio, dictionary, symbol, manifest, public
  header, API, phoneme, or timing delta, stop and treat the investigation as an
  extra-high reasoning checkpoint.
- Do not update golden baselines unless a phase explicitly allows it and the
  extra-high reasoning gate is active.
- Do not claim behavior preservation unless the relevant checks were run.

## Reasoning Sequence

This plan is intentionally ordered to avoid switching reasoning levels back and
forth.

- Phases 1 through 6 are high-reasoning phases.
- Phases 7 through 13 are extra-high-reasoning phases.
- If execution starts or remains at extra-high reasoning, phases 1 through 13
  may proceed without any reasoning-change prompts because extra-high satisfies
  the high-reasoning phases.
- If execution starts at high reasoning, stop once before Phase 7, call out the
  need to switch to extra-high reasoning, and wait for explicit approval.
- After Phase 7 begins, do not prompt for further reasoning changes in this
  plan unless an ambiguity, failure, or regression requires interruption.
- This plan never requires switching back from extra-high to high before
  completion.

## Extra-High Checkpoint Scope

Extra-high reasoning is required before:

- accepting or updating any golden behavior baseline
- adding non-US, phoneme, public API, callback, queue, threading, or timing
  baselines that may expose previously untracked differences
- changing CMake source membership, generated dictionary flow, staged packaging
  layout, live-audio option modeling, or multi-language `libtts.so` behavior
- changing public headers, public API signatures, calling conventions, exported
  symbols, installed header layout, or ABI-sensitive structures
- changing API implementation near `ttsapi.c`, `tts.h`, `ttsapi.h`, `init.c`,
  callback paths, or multi-language loader behavior
- changing medium-risk warnings involving conversions, pointer qualifiers,
  pointer/integer casts, callback signatures, thread function signatures,
  `volatile`, structure layout, or size truncation
- editing synthesis, phoneme, LTS, VTM, HLSYN, public API, audio, or threading
  code for warning cleanup
- wiring `src/platform` thread, mutex, event, filesystem, time, or audio
  wrappers into existing runtime code
- changing `src/dapi/src/nt/opthread.c` or `src/dapi/src/nt/linux_audio.c`
- simplifying sound-critical or language-selection macros
- quarantining historical target source branches beyond build-system opt-in
- promoting CMake to a primary Linux build path

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
- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/<phase>-cmake --symbol-mode name-type --expected tests/golden`
- `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out baseline-runs/<phase>/dist-manifest-detailed.txt`
- `tools/baseline/compare_manifest.sh --expected tests/golden/dist-manifest-detailed.txt --actual baseline-runs/<phase>/dist-manifest-detailed.txt --out baseline-runs/<phase>/dist-manifest-detailed.diff`
- `tools/baseline/check_public_headers.sh --out baseline-runs/<phase>/public-headers --expected tests/golden/public-headers`
- `tools/baseline/check_api_smoke.sh --out baseline-runs/<phase>/api-smoke`
- `tools/baseline/summarize_warnings.py --log baseline-runs/<phase>/build/build.log --out-dir baseline-runs/<phase>/warnings-default`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/<phase>/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/<phase>/warning-budget.tsv`

## Phase 1: Post-Merge Baseline Refresh

Status: completed

Reasoning checkpoint: high.

Goals:

- Confirm the local tree is synchronized with merged `origin/develop`.
- Re-run the accepted Linux baseline from the merged branch.
- Establish the starting warning, symbol, dictionary, audio, API, manifest, and
  CMake state for this plan.

Required work:

- Ensure local `develop` tracks merged `origin/develop`.
- Run the Autotools verification gate against `tests/golden`.
- Run the CMake subset verification gate with default exact local checks.
- Capture current warning counts and compare them to the previous final
  readiness values.
- Document the refreshed baseline in `docs/modernization/READINESS_REVIEW.md`
  or a new modernization note.

Success criteria:

- Local tree is clean before modifications begin.
- Accepted Autotools baselines reproduce.
- Accepted CMake subset baselines reproduce.
- Warning counts and any deltas are recorded.
- No source behavior changes are made.

Rules:

- This phase is a synchronization and verification checkpoint only.
- Do not update golden files in this phase.

Implementation Summary:

- Confirmed local `develop` is based on merged `origin/develop` at
  `0555a8fef13d39f7e31ac96bd582ffb05f34db98` with one local planning commit,
  `bf6e56912a5fb9c40de35b11b721c5e42cf0255b`.
- Updated `docs/modernization/READINESS_REVIEW.md` with the accelerated plan
  Phase 1 post-merge baseline refresh.
- Files changed: `docs/modernization/READINESS_REVIEW.md` and `PLAN.md`.
- Verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase1-post-merge --expected tests/golden`.
- Verification results: default warning-line count `1,778`, strict warning-line
  count `29,762`, parser-visible default warnings `1,757`, and warning budget
  status `ok`.
- Public headers matched allowlists. Exported symbols matched committed symbol
  baselines. Detailed Autotools manifest matched the committed baseline.
  Generated dictionaries and the US user-dictionary fixture matched committed
  baselines. Public API smoke output matched the committed speaker 0 golden WAV
  exactly. One-shot US English golden WAVs matched for speakers 0 through 8.
  Expanded US audio suites matched for speakers 0 through 8.
- Additional verification run:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase1-post-merge-cmake --expected tests/golden`.
- Additional verification results: CMake configured and built the staged
  target; `compile_commands.json` had `3,295` lines; generated dictionaries
  matched; one-shot and expanded US audio matched; `libtts.so` exported symbols
  matched exactly; language-library exported symbol name/type sets matched; the
  expanded platform smoke passed with `event_semantics=ok`; the CMake detailed
  staged manifest had `1,126` lines.
- Warning counts changed only by observation: default warnings and
  parser-visible default warnings matched the previous final readiness values;
  strict warning-line count decreased from `29,764` to `29,762`.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: low. This phase changed documentation only after
  verification passed.
- Known limitations: CMake remains side-by-side; CMake detailed metadata/hash
  parity, live audio hardware, callback timing, backend device selection,
  non-current platform builds, non-US audio, and phoneme/text output remain
  outside the verified behavior claims.

## Phase 2: Knowledge Base Reconciliation

Status: completed

Reasoning checkpoint: high.

Goals:

- Bring the documentation forward from "before PR publication" wording to the
  post-PR #4 merged state.
- Keep the modernization knowledge base internally consistent before new work
  begins.

Required work:

- Review `docs/modernization/READINESS_REVIEW.md`,
  `docs/modernization/README.md`, `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`,
  `docs/modernization/PLATFORM_WRAPPER_DECISION.md`, and
  `docs/modernization/NEXT_PLAN_GATE_MAP.md`.
- Update only stale facts that affect this plan's execution.
- Record remaining deferred areas without turning them into approved work.

Success criteria:

- Documentation accurately identifies PR #4 as merged.
- Autotools remains described as authoritative.
- CMake remains described as side-by-side.
- Deferred high-risk areas remain explicit.
- No code, build script, golden artifact, or runtime behavior change is made.

Rules:

- Documentation-only phase.
- Do not change phase gates or accepted baselines unless Phase 1 revealed a
  factual mismatch that must be recorded.

Implementation Summary:

- Reconciled the modernization knowledge base with the post-PR #4 state.
- Updated `docs/modernization/READINESS_REVIEW.md` so the previous plan is
  described as published and merged through PR #4 rather than awaiting hosted
  PR CI.
- Updated `docs/modernization/README.md` to describe readiness reviews as
  cumulative, the active gate map as the active-plan map, and the platform
  wrapper decision document as the current runtime-wiring decision record.
- Replaced the stale previous-plan phase table in
  `docs/modernization/NEXT_PLAN_GATE_MAP.md` with the active accelerated plan's
  Phase 1 through Phase 13 target map, required gates, throughput strategy, and
  deferred areas.
- Files changed: `docs/modernization/READINESS_REVIEW.md`,
  `docs/modernization/README.md`,
  `docs/modernization/NEXT_PLAN_GATE_MAP.md`, and `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because this phase was documentation-only.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase changed documentation only and did not
  modify source, build scripts, golden artifacts, or runtime behavior.
- Known limitations: this phase did not run new build gates beyond the Phase 1
  accepted gates and did not change the deferred status of high-risk runtime,
  audio, callback, queue, non-US, phoneme/text, CMake-promotion, or historical
  target work.

## Phase 3: Warning Inventory Refresh And Candidate Selection

Status: completed

Reasoning checkpoint: high.

Goals:

- Refresh the warning inventory from the current merged tree.
- Select one narrow low-risk warning cleanup target for Phase 4.

Required work:

- Use the Phase 1 default and strict warning summaries.
- Inspect warning counts by file, flag, and risk class.
- Prefer a private auxiliary tool, sample, or maintenance path.
- Select one warning category and one ownership area only.
- Update `docs/modernization/WARNING_INVENTORY.md` or a focused warning note
  with the selected candidate, verification gates, and rejected candidates.

Success criteria:

- The Phase 4 target is explicit and low risk.
- The selected target does not touch public headers, synthesis, phoneme, LTS,
  VTM, HLSYN, audio backend, runtime threading, callback, queue, or dictionary
  behavior.
- No warning cleanup is performed in this phase.

Rules:

- Use the stricter warning inventory when choosing the candidate.
- If no clearly low-risk candidate exists, document the blocker and skip Phase
  4 implementation rather than widening scope.

Implementation Summary:

- Generated a refreshed strict parser warning summary from the Phase 1 strict
  warning log with
  `tools/baseline/summarize_warnings.py --log baseline-runs/next3-phase1-post-merge/build/build-strict-warnings.log --out-dir baseline-runs/next3-phase3-warning-strict`.
- Updated `docs/modernization/WARNING_INVENTORY.md` with the accelerated plan
  Phase 3 counts, refreshed strict risk classification, selected Phase 4
  candidate, and rejected candidates.
- Selected Phase 4 candidate: `src/licunix/src/liceninc.c`
  `-Wmissing-prototypes`, limited to the private helper `all_digits` if source
  review confirms it can be made internal to the translation unit.
- Explicitly rejected the same file's `-Wpointer-sign` warnings for this pass
  because pointer signedness is outside the low-risk warning-cleanup scope.
- Files changed: `docs/modernization/WARNING_INVENTORY.md` and `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because no source cleanup was performed in this
  phase.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase updated warning planning documentation
  only and did not modify source, build scripts, golden artifacts, or runtime
  behavior.
- Known limitations: the refreshed strict inventory still contains substantial
  high, medium, and unknown warning debt; Phase 4 is intentionally limited to
  one private tool file and one warning category.

## Phase 4: Low-Risk Auxiliary Warning Cleanup

Status: completed

Reasoning checkpoint: high.

Goals:

- Reduce one low-risk warning category selected in Phase 3.
- Add a warning-budget ratchet only if the selected warning category reaches
  zero for that file or ownership area.

Required work:

- Make the smallest source change that resolves the selected warning.
- Keep the edit local to the Phase 3 target.
- Update `tests/golden/warnings/default-cleaned.tsv` only for a category proven
  clean by the verification output.
- Update the relevant warning documentation.

Success criteria:

- The selected warning count is reduced as planned.
- New warning-budget rows prevent that warning from returning.
- The Autotools verification gate passes.
- Public headers, exported symbols, dictionaries, API smoke output, one-shot US
  audio, expanded US audio suites, and the detailed Autotools manifest still
  match accepted baselines.

Rules:

- Do not mix warning categories.
- Do not perform pointer signedness, pointer qualifier, callback, volatile,
  structure layout, public API, audio, threading, parser, or synthesis cleanup
  in this phase.

Implementation Summary:

- Reviewed the selected `src/licunix/src/liceninc.c` helper and confirmed the
  `all_digits` symbol is used only inside that translation unit for this tool.
- Changed `src/licunix/src/liceninc.c` to make `all_digits` `static`, keeping
  the existing parameter type and function body unchanged.
- Added a zero-count warning-budget row for `src/licunix/src/liceninc.c`
  `-Wmissing-prototypes` in `tests/golden/warnings/default-cleaned.tsv`.
- Updated `docs/modernization/WARNING_INVENTORY.md` with the Phase 4 cleanup
  result and verification evidence.
- Files changed: `src/licunix/src/liceninc.c`,
  `tests/golden/warnings/default-cleaned.tsv`,
  `docs/modernization/WARNING_INVENTORY.md`, and `PLAN.md`.
- Verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase4-liceninc-warning --expected tests/golden`.
- Verification results: default warning-line count `1,778`, strict warning-line
  count `29,761`, parser-visible default warnings `1,757`, and warning budget
  status `ok`. Public headers, exported symbols, detailed Autotools manifest,
  dictionaries, user dictionaries, API smoke WAV, one-shot US audio, and
  expanded US audio suites all matched accepted baselines.
- Additional strict parser verification:
  `tools/baseline/summarize_warnings.py --log baseline-runs/next3-phase4-liceninc-warning/build/build-strict-warnings.log --out-dir baseline-runs/next3-phase4-warning-strict`.
  Parser-visible strict warnings decreased from `29,743` to `29,738`, and the
  refreshed strict parser reported no remaining warnings for
  `src/licunix/src/liceninc.c`.
- Additional warning-budget verification:
  `tools/baseline/check_warning_budgets.py --warnings baseline-runs/next3-phase4-liceninc-warning/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/next3-phase4-liceninc-warning/warning-budget-after.tsv`.
  Result: `warning_budget_status=ok`.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: low. The source change only narrowed a private helper's
  linkage in a license maintenance tool and did not alter runtime synthesis,
  public headers, exported symbols, dictionaries, audio, API behavior, or
  threading behavior.
- Known limitations: the warning-budget checker uses the default warning
  summary; strict warning evidence was captured separately for this strict-only
  cleanup. Pointer-sign warnings and other categories remain deferred.

## Phase 5: Deterministic Coverage Gap Map

Status: completed

Reasoning checkpoint: high.

Goals:

- Decide which deterministic behavior coverage gaps are ready for extra-high
  implementation later in this plan.
- Keep unavailable or unstable capture paths documented rather than forced.

Required work:

- Review existing public API smoke coverage, phoneme/text-mode feasibility,
  CMake subset verification, audio fixtures, and platform wrapper smoke
  coverage.
- Identify public-facing capture paths first. Capture broader internal data
  only when it directly helps verify functionality was not lost.
- Keep US English, all 9 speakers, one-shot, WAV output as the default audio
  baseline shape unless an extra-high phase explicitly expands coverage.
- Update the relevant modernization note with accepted candidates and deferred
  candidates.

Success criteria:

- Phase 7 public API smoke expansion scope is explicit.
- Phase 8 legacy `OP_*` parity scope is explicit.
- Phoneme/text-mode, non-US audio, callback timing, and live-audio gaps are
  either assigned to a later extra-high phase or explicitly deferred.
- No new baseline is accepted in this phase.

Rules:

- Documentation and planning only.
- Do not change parser, phoneme, synthesis, audio, API implementation, or
  runtime threading code.

Implementation Summary:

- Reviewed current public API smoke coverage, accepted US audio suites, phoneme
  baseline feasibility notes, CMake subset verification, and platform wrapper
  smoke coverage.
- Updated `docs/modernization/BASELINE_EXPANSION_PLAN.md` with the accelerated
  plan Phase 5 deterministic coverage gap map.
- Accepted Phase 7 candidate: expand the public API smoke harness through
  installed public headers/libraries only, keeping deterministic no-live-audio
  scalar/status/error-path checks and preserving the final WAV output.
- Accepted Phase 8 candidate: add isolated legacy `OP_*` parity evidence only
  if it can be tested without routing runtime behavior through `src/platform`.
- Accepted Phase 10 candidate: model CMake live-audio option state as
  side-by-side configuration evidence only, without opening audio devices or
  changing runtime routing.
- Deferred phoneme/text-mode golden baselines, non-US audio baselines,
  callback ordering, in-memory phoneme arrays, queue timing, pipe behavior, and
  live-audio backend behavior.
- Files changed: `docs/modernization/BASELINE_EXPANSION_PLAN.md` and
  `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because no source cleanup was performed in this
  phase.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase changed planning documentation only and
  did not modify source, build scripts, golden artifacts, parser behavior,
  phoneme output, audio behavior, API implementation, or runtime threading.
- Known limitations: this phase selected coverage candidates but did not add
  new tests or baselines; the deferred areas remain outside verified behavior
  claims.

## Phase 6: CMake Detailed Parity Policy

Status: completed

Reasoning checkpoint: high.

Goals:

- Define the evidence needed before any future CMake promotion decision.
- Separate accepted basic path/type parity from unresolved detailed
  metadata-hash differences.

Required work:

- Recompare current Autotools and CMake detailed manifests if Phase 1 produced
  fresh manifests.
- Review language-library symbol name/type parity versus full symbol
  address/order differences.
- Review CMake's missing live-audio option surface.
- Update `docs/modernization/CMAKE_OVERVIEW.md` and
  `docs/modernization/PACKAGING_LAYOUT.md` with a clear acceptance policy for
  detailed metadata/hash differences and promotion blockers.

Success criteria:

- CMake basic path/type parity remains recognized as achieved.
- Detailed metadata/hash differences are classified and not accidentally
  accepted as full parity.
- Live-audio option parity work in Phase 10 has explicit boundaries.
- CMake remains side-by-side.

Rules:

- Documentation and comparison phase only.
- Do not change `CMakeLists.txt`.
- Do not promote CMake.

Implementation Summary:

- Compared the accelerated Phase 1 Autotools detailed manifest with the
  accelerated Phase 1 CMake detailed manifest:
  `tools/baseline/compare_manifest.sh --expected baseline-runs/next3-phase1-post-merge/dist-manifest-detailed.txt --actual baseline-runs/next3-phase1-post-merge-cmake/dist-manifest-detailed.txt --out baseline-runs/next3-phase6-cmake-detailed-vs-autotools.diff`.
- Rechecked the CMake detailed manifest against the committed Autotools golden:
  `tools/baseline/compare_manifest.sh --expected tests/golden/dist-manifest-detailed.txt --actual baseline-runs/next3-phase1-post-merge-cmake/dist-manifest-detailed.txt --out baseline-runs/next3-phase6-cmake-detailed-vs-golden.diff`.
- Both comparisons reported `manifest: different`; the difference remains
  detailed metadata, size, and hash output for CMake-built binaries plus
  `doc/DECtalk/html` directory metadata.
- Reviewed CMake language-library exported symbol name/type evidence from the
  Phase 1 CMake gate; all language-library name/type comparisons passed.
- Updated `docs/modernization/CMAKE_OVERVIEW.md` and
  `docs/modernization/PACKAGING_LAYOUT.md` with the accelerated Phase 6
  detailed parity policy.
- Files changed: `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`, and `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because no source cleanup was performed in this
  phase.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase changed documentation only and did not
  modify CMake source membership, install layout, build scripts, runtime code,
  golden artifacts, or CMake promotion state.
- Known limitations: CMake remains side-by-side; detailed binary metadata/hash
  parity and live-audio option parity remain unresolved promotion blockers.

## Phase 7: Public API Smoke Matrix Expansion

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Expand deterministic public API smoke coverage without changing public API
  behavior.
- Improve confidence before any later API-boundary warning cleanup.

Required work:

- Extend the public API smoke harness under `tools/baseline/` only.
- Prefer installed-header and installed-library tests that run without live
  audio hardware.
- Exercise additional stable API calls selected in Phase 5, such as capability,
  language, speaker, parameter, error-path, shutdown, and repeated-open/close
  behavior when deterministic.
- Keep output comparison public-facing where possible. Capture broader
  internal data only if it directly verifies that behavior has not been lost.
- Update API coverage documentation.

Success criteria:

- The expanded API smoke harness builds against installed public headers and
  libraries.
- API smoke output remains deterministic.
- Public headers and exported symbols match accepted baselines.
- Dictionaries, user dictionaries, one-shot US audio, expanded US audio suites,
  and the public API smoke WAV match accepted baselines.
- No public header, public API signature, calling convention, structure layout,
  or exported symbol is changed.

Rules:

- Do not touch `ttsapi.h`, `tts.h`, public headers, exported symbol lists, or
  runtime API implementation files unless this phase is explicitly revised.
- Stop on any API, symbol, header, dictionary, or audio delta.

Implementation Summary:

- Expanded `tools/baseline/api_smoke.c` only. No public headers, exported
  symbol baselines, or runtime API implementation files were changed.
- Added deterministic no-live-audio coverage for a second
  `TextToSpeechStartup()`/`TextToSpeechShutdown()` cycle, null output-pointer
  validation for `TextToSpeechGetRate()` and `TextToSpeechGetSpeaker()`,
  zero-count `TextToSpeechGetStatus()`, no-audio `STATUS_SPEAKING` and
  `WAVE_OUT_DEVICE_ID`, mixed no-audio status behavior,
  `TextToSpeechCloseInMemory()` when memory output is not open, and
  `TextToSpeechOpenInMemory()` invalid-format validation.
- Updated `docs/modernization/PUBLIC_API_AUDIT.md` with the expanded API smoke
  matrix and verification evidence.
- Files changed: `tools/baseline/api_smoke.c`,
  `docs/modernization/PUBLIC_API_AUDIT.md`, and `PLAN.md`.
- Targeted verification run:
  `tools/baseline/check_api_smoke.sh --out baseline-runs/next3-phase7-api-smoke/api-smoke`.
  Result: expanded API smoke compiled, ran, and produced an exact WAV match
  against `tests/golden/audio/us/speaker_0.wav`.
- Full verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase7-api-smoke --expected tests/golden`.
- Full verification results: default warning-line count `1,778`, strict
  warning-line count `29,761`, parser-visible default warnings `1,757`, and
  warning budget status `ok`. Public headers, exported symbols, detailed
  Autotools manifest, dictionaries, user dictionaries, API smoke WAV, one-shot
  US audio, and expanded US audio suites all matched accepted baselines.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: low to medium. The change expands test coverage for
  public API behavior but does not alter installed headers, exported symbols,
  runtime implementation, dictionaries, audio generation, live-audio routing,
  callbacks, queues, or threading behavior.
- Known limitations: this still is not a full API conformance suite and does
  not test live audio hardware, callbacks, in-memory buffers with returned
  samples, phoneme arrays, timing-sensitive behavior, non-US speech output, or
  queue/pipe behavior.

## Phase 8: Legacy OP_* Parity Harness

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Add evidence for legacy `OP_*` runtime semantics before considering any
  platform wrapper adapter or runtime wiring.
- Keep all runtime behavior unchanged.

Required work:

- Add a dedicated test or smoke harness that exercises legacy `OP_*` behavior
  without routing DECtalk runtime code through `src/platform`.
- Cover only behaviors that can be tested deterministically without live audio:
  thread create/join handle ownership, wait timeout behavior where available,
  priority get/set return behavior, `OP_Sleep(0)` yield behavior at smoke-test
  level, event auto-reset behavior, and lightweight lock timeout polling.
- Compare the covered legacy semantics against the existing `dt_platform_smoke`
  coverage and record gaps.
- Update `docs/modernization/PLATFORM_WRAPPER_DECISION.md`.

Success criteria:

- The new parity harness is isolated from runtime synthesis and live audio.
- CMake and/or Autotools verification runs the harness in a deterministic way.
- Existing Autotools and CMake gates still pass.
- Runtime `opthread.c`, `linux_audio.c`, audio routing, callbacks, queues,
  pipes, and public APIs are not changed.

Rules:

- Do not wire `src/platform` wrappers into runtime code.
- Do not change `src/dapi/src/nt/linux_audio.c`.
- Do not change `src/dapi/src/nt/opthread.c` unless the phase is explicitly
  revised with a narrower approved edit and matching rollback plan.
- If direct `OP_*` harnessing requires invasive runtime changes, document the
  blocker and stop the implementation.

Implementation Summary:

- Added `tools/platform/opthread_smoke.c`, a documented developer smoke harness
  that links directly against `src/dapi/src/nt/opthread.c` to record selected
  legacy `OP_*` behavior without routing runtime code through `src/platform`.
- Added a CMake-only `opthread_smoke` target in `CMakeLists.txt`.
- Updated `tools/baseline/verify_cmake_subset.sh` to build and run
  `opthread_smoke`, recording output in
  `baseline-runs/<phase>/opthread-smoke.log` and listing it in the verifier
  summary.
- Covered default-stack thread create, current Linux
  `OP_WaitForThreadTermination` return behavior, thread return value, priority
  get/set, `OP_Sleep(0)`, mutex lock/unlock, auto-reset/manual-reset event
  semantics, and lightweight lock zero-timeout behavior.
- Updated `docs/modernization/PLATFORM_WRAPPER_DECISION.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`, and `src/platform/README.md` with
  the new evidence and limitations.
- Files changed: `tools/platform/opthread_smoke.c`, `CMakeLists.txt`,
  `tools/baseline/verify_cmake_subset.sh`,
  `docs/modernization/PLATFORM_WRAPPER_DECISION.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`, `src/platform/README.md`, and
  `PLAN.md`.
- CMake verification run:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase8-opthread-cmake --expected tests/golden`.
- CMake verification results: generated dictionaries matched; one-shot US audio
  matched; expanded US audio suites matched; `libtts.so` exported symbols
  matched exactly; language-library exported symbol name/type sets matched;
  `dt_platform_smoke` passed; `opthread_smoke` passed; `compile_commands.json`
  had `3,307` lines; detailed CMake staged manifest had `1,126` lines.
- `opthread_smoke` recorded:
  `op_thread_wait_status=1`, `op_thread_return=77`,
  `op_thread_priority=0`, `op_mutex=ok`, `op_event_semantics=ok`,
  `op_lightweight_lock=ok`, `op_sleep_zero=ok`, and `opthread_smoke=ok`.
- Autotools verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase8-opthread-autotools --expected tests/golden`.
- Autotools verification results: default warning-line count `1,778`, strict
  warning-line count `29,760`, parser-visible default warnings `1,757`, and
  warning budget status `ok`. Public headers, exported symbols, detailed
  Autotools manifest, dictionaries, user dictionaries, API smoke WAV, one-shot
  US audio, and expanded US audio suites all matched accepted baselines.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: medium. This phase added a runtime-adjacent test target
  that compiles `opthread.c`, but it did not modify `opthread.c`,
  `linux_audio.c`, runtime libraries, installed headers, exported symbols,
  audio routing, callbacks, queues, pipes, or threading behavior.
- Known limitations: the harness does not prove live-audio, callback, queue,
  pipe, buffer ownership, reset/pause/restart, backend state, exact scheduler
  fairness, timing guarantees, non-current platform behavior, or runtime
  wrapper replacement safety.

## Phase 9: Platform Wrapper Adapter Decision

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Decide whether `src/platform` needs adapter APIs to preserve legacy runtime
  contracts exactly.
- Avoid premature runtime migration.

Required work:

- Review Phase 8 legacy `OP_*` evidence against `dt_thread`, `dt_mutex`,
  `dt_event`, and `dt_time` semantics.
- If evidence supports a safe adapter scaffold, add it only as isolated
  CMake-smoke scaffolding under `src/platform/` with standard file
  documentation.
- If evidence is incomplete, document the missing semantics and defer adapter
  implementation.
- Update platform wrapper documentation.

Success criteria:

- A clear adapter decision is recorded.
- Any new source/header file has standard project documentation at the top.
- No runtime code is routed through `src/platform`.
- Existing Autotools and CMake gates still pass when relevant.

Rules:

- Adapter scaffolding is allowed only if it is not linked into DECtalk runtime
  libraries or installed public API surfaces.
- Do not change live audio behavior, callback behavior, queue behavior, thread
  lifecycle behavior, or public API behavior.

Implementation Summary:

- Reviewed the accelerated Phase 8 `opthread_smoke` evidence against the
  current `dt_thread`, `dt_mutex`, `dt_event`, and `dt_time` wrapper semantics.
- Decision: defer platform adapter scaffolding. The current wrappers do not
  encode legacy `OP_*` handle ownership, Linux wait return behavior, priority
  get/set behavior, `ThreadLock` timeout-polling behavior, or exact `OP_WAIT_*`
  compatibility.
- Updated `docs/modernization/PLATFORM_WRAPPER_DECISION.md` with the Phase 9
  adapter decision, required future evidence, and reasons for deferral.
- No new adapter source or header was created.
- Files changed: `docs/modernization/PLATFORM_WRAPPER_DECISION.md` and
  `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because this phase was documentation-only.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase changed documentation only after the
  Phase 8 CMake and Autotools gates passed.
- Known limitations: runtime wrapper wiring, live audio, callbacks, queues,
  pipes, buffer ownership, reset/pause/restart behavior, exact timing, and
  non-current platform `OP_*` behavior remain outside verified coverage.

## Phase 10: CMake Live-Audio Option Parity Scaffolding

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Model the Autotools live-audio option surface in side-by-side CMake without
  promoting CMake or changing deterministic runtime behavior.

Required work:

- Add or document CMake options only when their default behavior is explicitly
  understood.
- Prefer explicit `DISABLE_AUDIO`, `USE_ALSA`, and `USE_PULSEAUDIO` modeling
  that preserves the current verified no-hardware deterministic checks.
- Update compile-command or configuration evidence so the selected audio option
  state is visible.
- Update `docs/modernization/AUDIO_BACKEND.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`, and macro documentation as needed.

Success criteria:

- CMake configures and builds with the selected default audio option state.
- The CMake subset verifier passes.
- The Autotools verifier passes.
- Deterministic WAV, dictionary, symbol, public header, API smoke, and manifest
  checks remain stable.
- CMake remains side-by-side.

Rules:

- Do not open live audio devices in tests.
- Do not route runtime audio through new wrappers.
- Do not change `src/dapi/src/nt/linux_audio.c`.
- Stop on any unexplained audio, symbol, manifest, compile-command, or
  dictionary delta.

Implementation Summary:

- Added side-by-side CMake cache options for the Autotools audio macro surface:
  `DECTALK_CMAKE_DISABLE_AUDIO`, `DECTALK_CMAKE_USE_ALSA`, and
  `DECTALK_CMAKE_USE_PULSEAUDIO`.
- The options define `DISABLE_AUDIO`, `USE_ALSA`, and `USE_PULSEAUDIO` for
  CMake targets when enabled. `DISABLE_AUDIO` takes precedence over ALSA and
  PulseAudio macro options.
- Preserved the default verified CMake state with all new options `OFF`.
- Updated `docs/modernization/AUDIO_BACKEND.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`, and
  `docs/modernization/MACRO_INVENTORY.md` with the option surface, default
  metadata, disabled-audio metadata probe, and limitations.
- Files changed: `CMakeLists.txt`, `docs/modernization/AUDIO_BACKEND.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/MACRO_INVENTORY.md`, and `PLAN.md`.
- CMake default verification run:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase10-cmake-audio-options --expected tests/golden`.
- CMake default verification results: generated dictionaries matched; one-shot
  US audio matched; expanded US audio suites matched; `libtts.so` exported
  symbols matched exactly; language-library exported symbol name/type sets
  matched; `dt_platform_smoke` passed with `audio_disabled=0`, `audio_oss=1`,
  `audio_alsa=0`, and `audio_pulseaudio=0`; `opthread_smoke` passed;
  `compile_commands.json` had `3,307` lines; detailed CMake staged manifest had
  `1,126` lines.
- Metadata-only disabled-audio probe:
  `cmake -S . -B baseline-runs/next3-phase10-cmake-disable-audio/build -DCMAKE_BUILD_TYPE=Release -DDECTALK_CMAKE_DISABLE_AUDIO=ON -DCMAKE_C_COMPILER=/usr/bin/gcc`,
  `cmake --build baseline-runs/next3-phase10-cmake-disable-audio/build --target dt_platform_smoke -- -j1`,
  and
  `baseline-runs/next3-phase10-cmake-disable-audio/build/dt_platform_smoke /home/yam/dt PLAN.md`.
  Result: `audio_disabled=1`, `audio_oss=0`, `audio_alsa=0`,
  `audio_pulseaudio=0`, and `audio_audioqueue=0`.
- Autotools verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase10-cmake-audio-options-autotools --expected tests/golden`.
- Autotools verification results: default warning-line count `1,778`, strict
  warning-line count `29,760`, parser-visible default warnings `1,757`, and
  warning budget status `ok`. Public headers, exported symbols, detailed
  Autotools manifest, dictionaries, user dictionaries, API smoke WAV, one-shot
  US audio, and expanded US audio suites all matched accepted baselines.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: medium. This phase changes side-by-side CMake
  configuration only; defaults preserve verified behavior, but non-default
  live-audio macro combinations are not promoted or runtime-certified.
- Known limitations: this phase does not replace Autotools audio probing, does
  not open live audio devices, does not link ALSA or PulseAudio libraries, does
  not route runtime audio through wrappers, and does not validate live-audio
  callbacks, queues, timing, or backend device selection.

## Phase 11: API-Boundary Warning Pilot

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Use the expanded API smoke coverage from Phase 7 to evaluate one
  API-adjacent warning cleanup.
- Reduce warning debt only if the candidate is narrow and behavior-neutral.

Required work:

- Select one API-boundary warning candidate from the refreshed warning
  inventory.
- Prefer local implementation hygiene such as missing includes, missing private
  prototypes, or indentation/control-flow clarity.
- Avoid public API signatures, callbacks, structure layout, calling convention,
  pointer/integer conversions, `volatile`, thread lifecycle, and loader
  behavior unless a separate explicit approval is added.
- Add a warning-budget row only for a category proven clean.
- Update `docs/modernization/API_BOUNDARY_WARNINGS.md` or related notes.

Success criteria:

- The selected warning is reduced or the phase documents why it was deferred.
- Expanded API smoke, public headers, exported symbols, dictionaries, one-shot
  US audio, expanded US audio suites, and warning budgets pass.
- No public API or ABI surface changes.

Rules:

- Stop on any API smoke, symbol, public header, dictionary, or audio delta.
- If the only available candidates involve callbacks, ABI-sensitive structs,
  pointer/integer casts, or runtime threading, defer instead of widening scope.

Implementation Summary:

- Selected the `src/dapi/src/kernel/services.c` `-Wmissing-prototypes`
  warning cluster reported through the API build context as
  `src/dapi/src/api/services.c`.
- Added matching local prototypes above the existing service-function
  definitions. No symbol was made `static`, and no function definition, public
  header, public API signature, calling convention, structure layout, call
  site, callback path, loader path, thread path, or audio path was changed.
- Added a zero-count warning-budget row for
  `src/dapi/src/kernel/services.c` `-Wmissing-prototypes`.
- Refreshed `tests/golden/dist-manifest-detailed.txt` after the first
  verification pass showed expected metadata-hash changes for installed
  language libraries and sample demos linked with the touched source. The
  install path/type layout did not change compared with the Phase 10 Autotools
  capture.
- Updated `docs/modernization/API_BOUNDARY_WARNINGS.md` and
  `docs/modernization/WARNING_INVENTORY.md` with the selected candidate,
  verification evidence, and deferred warning categories.
- Files changed: `src/dapi/src/kernel/services.c`,
  `tests/golden/warnings/default-cleaned.tsv`,
  `tests/golden/dist-manifest-detailed.txt`,
  `docs/modernization/API_BOUNDARY_WARNINGS.md`,
  `docs/modernization/WARNING_INVENTORY.md`, and `PLAN.md`.
- Initial verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase11-api-boundary-services --expected tests/golden`.
  Result: all captures completed, exported symbols matched, API smoke and
  deterministic US audio matched, but the detailed manifest comparison stopped
  on expected binary metadata-hash changes from the source edit.
- Additional classification checks: generated dictionaries and user
  dictionaries matched; public headers matched; warning budget passed; strict
  warning parser output in
  `baseline-runs/next3-phase11-api-boundary-services/warnings-strict/`
  showed no remaining `src/dapi/src/kernel/services.c`
  `-Wmissing-prototypes` rows.
- Final current-tree verification run after refreshing the detailed manifest
  baseline and normalizing the inserted prototype block:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase11-api-boundary-services-lf --expected tests/golden`.
- Final verification results: default warning-line count `1,777`, strict
  warning-line count `29,665`, parser-visible default warnings `1,757`,
  parser-visible strict warnings `29,644`, and warning budget status `ok`.
  Public headers, exported symbols, detailed Autotools manifest, dictionaries,
  user dictionaries, API smoke WAV, one-shot US audio, and expanded US audio
  suites all matched accepted baselines.
- Public exports did not change according to the committed symbol
  comparisons. Dictionaries did not change according to the committed
  dictionary comparisons. Golden audio did not change according to the
  committed one-shot and expanded deterministic WAV comparisons.
- Behavior risk level: medium. The source edit is prototype-only and preserves
  linkage, but the touched source is linked into installed runtime libraries,
  so the binary metadata/hash baseline changed and was refreshed after behavior
  gates passed.
- Known limitations: pointer-sign, pointer-qualifier, unused-parameter,
  callback-facing, loader-adjacent, structure-layout, public API
  implementation, threading, and audio warnings remain deferred.

## Phase 12: CMake Promotion Readiness Decision

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Decide whether CMake is closer to promotion after Phase 10.
- Avoid promoting CMake inside this plan unless a separate explicit approval is
  added.

Required work:

- Review CMake basic path/type parity, detailed metadata/hash differences,
  language-library symbol name/type evidence, compile-command evidence, and
  audio-option parity evidence.
- Compare current CMake and Autotools staged manifests.
- Record whether remaining differences are eliminated, explicitly acceptable,
  or still blocking.
- Update `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`, and the readiness review.

Success criteria:

- CMake status is explicit and evidence-based.
- Promotion blockers are listed with required future gates.
- If CMake remains side-by-side, the reason is concrete.
- If CMake appears promotable, create a future promotion plan rather than
  promoting it in this phase.

Rules:

- Do not promote CMake in this phase.
- Do not remove Autotools or existing build paths.
- Do not change install layout unless explicitly approved by a revised phase.

Implementation Summary:

- Rechecked current CMake readiness after Phase 10 audio-option modeling and
  the Phase 11 API-boundary warning pilot.
- Ran
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase12-cmake-readiness --expected tests/golden`.
- CMake verification results: generated dictionaries matched; one-shot US audio
  matched for speakers 0 through 8; expanded US audio suites matched for
  speakers 0 through 8; CMake-staged `libtts.so` matched the committed exact
  exported-symbol baseline; language-library exported symbol name/type sets
  matched; `dt_platform_smoke` passed with default audio metadata;
  `opthread_smoke` passed; `compile_commands.json` had `3,307` lines; and the
  detailed CMake staged manifest had `1,126` lines.
- Captured the current CMake path/type manifest and compared it with the
  current Phase 11 Autotools path/type manifest:
  `tools/baseline/compare_manifest.sh --expected baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest.txt --actual baseline-runs/next3-phase12-cmake-readiness/dist-manifest.txt --out baseline-runs/next3-phase12-cmake-basic-vs-autotools.diff`.
  Result: `manifest: ok`, with `589` entries on each side.
- Compared the current CMake detailed metadata-hash manifest with the current
  Phase 11 Autotools detailed metadata-hash manifest:
  `tools/baseline/compare_manifest.sh --expected baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest-detailed.txt --actual baseline-runs/next3-phase12-cmake-readiness/dist-manifest-detailed.txt --out baseline-runs/next3-phase12-cmake-detailed-vs-autotools.diff`.
  Result: `manifest: different`, with `1,126` entries on each side.
- Decision: CMake remains side-by-side and is not promoted. Basic path/type
  parity is still accepted, but detailed binary metadata/hash differences,
  `doc/DECtalk/html` directory metadata, unaccepted full language-library
  symbol address/order differences, and unverified live-audio behavior remain
  promotion blockers.
- Updated `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`, and
  `docs/modernization/READINESS_REVIEW.md`.
- Files changed: `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PACKAGING_LAYOUT.md`,
  `docs/modernization/READINESS_REVIEW.md`, and `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change because this phase was documentation and
  comparison only.
- Public exports did not change. Dictionaries did not change. Golden audio did
  not change.
- Behavior risk level: low. This phase changed documentation only and did not
  modify source, build scripts, install layout, public APIs, dictionaries,
  audio output, or CMake promotion state.
- Known limitations: CMake promotion remains deferred; live audio hardware,
  callbacks, queue behavior, backend device selection, non-current targets,
  non-US audio, and phoneme/text output remain outside verified coverage.

## Phase 13: Final Readiness Review, PR, And Merge

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Record the final readiness state for this plan.
- Publish and merge the completed work.

Required work:

- Run the final Autotools verification gate.
- Run the final CMake subset verification gate.
- Capture and compare relevant manifests.
- Record final warning counts, warning budgets, public headers, exported
  symbols, dictionaries, user dictionaries, public API smoke, deterministic
  audio, CMake status, platform wrapper status, and deferred risks.
- Update `docs/modernization/READINESS_REVIEW.md`.
- Update this plan with the final Implementation Summary.
- Commit final documentation and any remaining intended files.
- Create a pull request with an appropriate body.
- Merge the pull request once required checks and repository policy allow it.

Success criteria:

- Final Autotools verification passes.
- Final CMake subset verification passes.
- Warning budgets pass.
- No unexplained behavior, symbol, header, dictionary, manifest, API, or audio
  deltas remain.
- The PR is created and merged, or a repository-policy blocker is documented
  with exact status.

Rules:

- Do not claim coverage for live audio hardware, callbacks, queue timing,
  backend device selection, non-current targets, non-US audio, or phoneme/text
  output unless this plan actually adds and runs those checks.
- Do not merge if required checks fail.

Implementation Summary:

- Ran the final authoritative Autotools/Linux verification gate:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase13-final --expected tests/golden`.
- Final Autotools results: default warning-line count `1,778`, strict
  warning-line count `29,665`, parser-visible default warnings `1,756`,
  parser-visible strict warnings `29,643`, and warning budget status `ok`.
  Public headers, exported symbols, detailed Autotools manifest, dictionaries,
  user dictionaries, API smoke WAV, one-shot US audio, and expanded US audio
  suites all matched accepted baselines.
- Ran the final CMake subset verification gate:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase13-final-cmake --expected tests/golden`.
- Final CMake results: generated dictionaries matched; one-shot US audio
  matched for speakers 0 through 8; expanded US audio suites matched for
  speakers 0 through 8; CMake-staged `libtts.so` matched the committed exact
  exported-symbol baseline; language-library exported symbol name/type sets
  matched; `dt_platform_smoke` passed; `opthread_smoke` passed;
  `compile_commands.json` had `3,307` lines; and the detailed CMake staged
  manifest had `1,126` lines.
- Captured and compared final CMake path/type and detailed manifests against
  the final Autotools manifests. Path/type staging matched with `589` entries
  on each side. Detailed metadata-hash comparison still differed with `1,126`
  entries on each side because CMake-built binary sizes/hashes and
  `doc/DECtalk/html` directory metadata still differ.
- Updated `docs/modernization/READINESS_REVIEW.md` with the final readiness
  review, final gate results, CMake status, behavior-risk boundaries, and
  remaining limitations.
- Files changed: `docs/modernization/READINESS_REVIEW.md` and `PLAN.md`.
- Verification command:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Public exports did not change according to the committed symbol comparisons.
  Dictionaries did not change according to the committed dictionary
  comparisons. Golden audio did not change according to the committed one-shot
  and expanded deterministic WAV comparisons.
- Behavior risk level: low for this final documentation checkpoint. The full
  plan's highest-risk accepted change was the Phase 11 API-boundary local
  prototype cleanup plus refreshed detailed Autotools manifest baseline, which
  passed final symbol, dictionary, header, API-smoke, audio, and warning-budget
  gates.
- Known limitations: live audio hardware, callbacks, queue timing, backend
  device selection, non-current targets, non-US audio, phoneme/text output,
  CMake promotion, runtime platform-wrapper wiring, and remaining high-risk
  warning cleanup are still deferred.
- PR publication and merge are performed after this committed plan update so
  the PR contains the completed readiness record.

## Definition Of Done For Each Phase

Each completed phase summary must state:

- files changed or created
- what was implemented
- verification commands and results
- whether warning counts changed
- whether public exports changed
- whether dictionaries changed
- whether golden audio changed
- behavior risk and remaining limitations

## Current Immediate Next Step

Before implementing Phase 1, review this plan against the current codebase and
ask any upfront clarification questions. If there are no blockers and execution
is already using extra-high reasoning, the plan can run from Phase 1 through
Phase 13 without reasoning-change prompts. If execution starts at high
reasoning, stop before Phase 7 for a single extra-high reasoning change prompt.
