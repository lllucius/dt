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
  - default build: 1,829
  - strict build: 29,854
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
- `tools/baseline/compare_audio.py --actual baseline-runs/<phase>/audio-us`
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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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

Status: not started

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
