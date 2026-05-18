# PLAN.md

## Objective

Continue DECtalk modernization after PR #6 while preserving current Linux
behavior.

This plan is an accelerated, warning-focused follow-on pass. Extra-high
reasoning is assumed for every phase, so there are no reasoning-change prompts
and no special sequencing for reasoning-level transitions.

The plan deliberately chooses cleanup targets outside synthesis, parser, public
API, live audio, threading, and dictionary-format code:

- refresh the post-PR #6 baseline;
- compress the current warning evidence into a new gate map;
- clean one low-risk unused-variable category in the user-dictionary
  alphabetizer tool;
- clean one low-risk missing-prototype category in the tunecheck sample tool;
- clean one low-risk unused-variable category in the tunecheck sample tool;
- run final readiness gates, create a PR, and merge it when GitHub accepts the
  merge.

This plan does not approve speech output changes, phoneme output changes,
dictionary behavior changes, public API changes, exported symbol changes,
install-layout changes, live-audio behavior changes, threading behavior
changes, CMake promotion, historical target deletion, or default runtime
wrapper replacement.

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

## Current Starting Facts After PR #6

- PR #6, `Complete next high-risk modernization plan`, was merged into
  `develop` as merge commit `5b408cbea26ba5d9697366e0a4169c86a5f424cc`.
- Local `develop` is synchronized with `origin/develop`.
- Autotools remains the authoritative Linux build path.
- The final PR #6 Autotools gate passed:
  `tools/baseline/verify_current.sh --run-dir
  baseline-runs/next4-phase13-final-default --expected tests/golden`.
- Final PR #6 warning counts were: default warning-line count `1,778`, strict
  warning-line count `29,593`, parser-visible default warnings `1,757`, and
  parser-visible strict warnings `29,571`.
- Default and strict warning budgets passed.
- Public headers, exported symbols, generated dictionaries, user dictionaries,
  public API smoke, API callback smoke, one-shot US English WAVs, expanded US
  audio suites, and non-US one-shot WAVs matched accepted baselines.
- The final PR #6 CMake subset gate passed:
  `tools/baseline/verify_cmake_subset.sh --run-dir
  baseline-runs/next4-phase13-final-cmake --expected tests/golden`.
- Final PR #6 CMake evidence included `compile_commands.json` with `3,325`
  lines, matching generated dictionaries, matching US one-shot and expanded
  audio, exact `libtts.so` exported-symbol parity, language-library symbol
  name/type parity, passing `dt_platform_smoke`, passing `opthread_smoke`, and
  passing `dt_opthread_adapter_smoke`.
- CMake path/type staged manifest parity matched Autotools exactly with `589`
  entries on each side.
- CMake detailed metadata-hash parity is still not accepted. The CMake and
  Autotools detailed manifests both have `1,126` entries, but CMake-built
  binary sizes/hashes and `doc/DECtalk/html` directory metadata still differ.
- CMake audio option rows remain metadata-only on this host; live-audio
  probing, backend linkage, device selection, callbacks, queues, timing, and
  live hardware behavior are not certified.
- `src/platform/` remains isolated scaffolding. Runtime wrapper wiring is still
  deferred.
- Deterministic phoneme/text golden baselines have not been accepted.

## Operating Rules

- Work phases strictly in order.
- Extra-high reasoning is active for every phase. Do not prompt for
  reasoning-level changes.
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
- Do not delete historical target code.
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
- Ask only when an ambiguity, missing external credential, destructive action,
  or unbounded behavior risk cannot be resolved from local context.
- If a phase finds an unexplained audio, dictionary, symbol, manifest, public
  header, API, phoneme, text, timing, or threading delta, stop and resolve it
  before continuing.
- Do not update golden baselines unless a phase explicitly allows it and the
  relevant before/after evidence is captured.
- Do not claim behavior preservation unless the relevant checks were run.

## Extra-High Scope

Extra-high reasoning is already active and required for the whole plan,
including:

- accepting or updating any golden behavior baseline;
- changing warning budgets;
- editing dictionary-generation tools or sample tools that are shipped in the
  staged tree;
- changing CMake source membership, generated dictionary flow, staged
  packaging layout, audio option modeling, or multi-language `libtts.so`
  behavior;
- changing public headers, public API signatures, calling conventions,
  exported symbols, installed header layout, or ABI-sensitive structures;
- editing synthesis, phoneme, LTS, VTM, HLSYN, public API, audio, or threading
  code for warning cleanup;
- changing `src/dapi/src/nt/opthread.c` or `src/dapi/src/nt/linux_audio.c`;
- simplifying sound-critical or language-selection macros;
- promoting CMake to a primary Linux build path.

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
- `tools/baseline/check_audio_option_matrix.sh --run-dir baseline-runs/<phase>-audio-options`
- `tools/baseline/check_public_headers.sh --out baseline-runs/<phase>/public-headers --expected tests/golden/public-headers`
- `tools/baseline/check_api_smoke.sh --out baseline-runs/<phase>/api-smoke`
- `tools/baseline/check_api_callback_smoke.sh --out baseline-runs/<phase>/api-callback-smoke`
- `tools/baseline/summarize_warnings.py --log baseline-runs/<phase>/build/build-strict-warnings.log --out-dir baseline-runs/<phase>/warnings-strict`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/<phase>/warnings-strict/warnings.tsv --budget tests/golden/warnings/strict-cleaned.tsv --out baseline-runs/<phase>/warning-budget-strict.tsv`

## Phase 1: Post-PR #6 Baseline Refresh

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Confirm the local tree is synchronized with merged `origin/develop`.
- Re-run the accepted Linux baseline from merged PR #6.
- Establish starting warning, symbol, dictionary, audio, API, callback,
  manifest, CMake, platform-smoke, OP-smoke, and adapter-smoke state for this
  plan.

Required work:

- Ensure local `develop` tracks merged `origin/develop`.
- Run the Autotools verification gate against `tests/golden`.
- Run the CMake subset verification gate.
- Record current warning counts and any deltas from the PR #6 final readiness
  state.
- Update `docs/modernization/READINESS_REVIEW.md` with the refreshed starting
  state.

Success criteria:

- Accepted Autotools baselines reproduce.
- Accepted CMake subset baselines reproduce.
- Warning counts and any deltas are recorded.
- No source behavior changes are made.

Rules:

- This phase is a synchronization and verification checkpoint only.
- Do not update golden files in this phase.

Implementation Summary:

- Confirmed local `develop` was synchronized with `origin/develop` at the
  merged PR #6 state before running verification.
- Ran the Autotools/current gate:
  `tools/baseline/verify_current.sh --run-dir
  baseline-runs/next5-phase1-post-pr6 --expected tests/golden`. Result:
  passed. Counts were 1,778 default warning lines, 29,593 strict warning
  lines, 1,757 parser-visible default warnings, and 29,572 parser-visible
  strict warnings. Default and strict warning budgets passed.
- The Autotools/current gate also matched accepted public headers, exported
  symbols, detailed manifest, generated dictionaries, user dictionaries, public
  API smoke, API callback smoke, US one-shot audio, expanded US audio suites,
  and non-US one-shot audio.
- Ran the CMake subset gate:
  `tools/baseline/verify_cmake_subset.sh --run-dir
  baseline-runs/next5-phase1-post-pr6-cmake --expected tests/golden`. Result:
  passed. CMake generated `compile_commands.json` with 3,325 lines; matched
  dictionaries, US one-shot audio, expanded US audio suites, exact `libtts.so`
  symbols, and language-library symbol name/type sets; and passed
  `dt_platform_smoke`, `opthread_smoke`, and `dt_opthread_adapter_smoke`.
- Updated `docs/modernization/READINESS_REVIEW.md` with the refreshed starting
  state. The parser-visible strict count is one row higher than the PR #6 final
  readiness note, but all warning budgets and accepted behavior gates passed.
- No source, public API, build script, runtime, dictionary-generation, audio,
  threading, golden baseline, or accepted behavior artifact was changed.

## Phase 2: Warning Candidate Gate Map

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Compress the post-PR #6 warning evidence into this plan's exact cleanup
  queue.
- Keep cleanup phases limited to one file and one warning category each.

Required work:

- Review strict warning evidence from Phase 1.
- Update `docs/modernization/NEXT_PLAN_GATE_MAP.md` for this plan.
- Update `docs/modernization/WARNING_INVENTORY.md` with the selected
  candidates and rejected candidates.
- Confirm the selected cleanup targets are outside runtime synthesis, parser,
  public API, live audio, threading, and dictionary-format behavior.

Success criteria:

- Each cleanup phase has a named file, warning category, verification gate, and
  rollback rule.
- No code, runtime behavior, build behavior, or golden artifacts are changed.

Rules:

- Documentation-only phase.
- Do not widen later phases beyond what can be verified deterministically.

## Phase 3: UDICT Alphabetizer Unused-Variable Cleanup

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Clean one low-risk strict warning category in `src/udicunix/src/alphabet.c`.
- Preserve user-dictionary generated output exactly.

Required work:

- Remove only unused local variables from `src/udicunix/src/alphabet.c`.
- Do not touch pointer signedness, text parsing, sorting, codepage conversion,
  file I/O, or dictionary output logic.
- Add a strict warning-budget row only if
  `src/udicunix/src/alphabet.c` `-Wunused-variable` reaches zero.
- Update warning documentation.

Success criteria:

- `src/udicunix/src/alphabet.c` has zero strict `-Wunused-variable` warnings.
- User dictionaries, generated dictionaries, public headers, symbols, API
  smoke, callback smoke, deterministic audio, manifests, and warning budgets
  pass.

Rules:

- Do not change dictionary format, dictionary sorting, dictionary lookup, or
  user-dictionary generated output.
- Do not mix pointer signedness or qualifier cleanup into this phase.

## Phase 4: Tunecheck Missing-Prototype Cleanup

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Clean one low-risk strict missing-prototype warning in the tunecheck sample
  tool.
- Keep the tunecheck command-line behavior and staged binaries intact except
  for expected rebuild metadata.

Required work:

- Review `src/samplosf/src/dtsamples/tunecheck.c` local function visibility.
- Make only the private `MakeTunerParams` helper file-local if review confirms
  no external reference.
- Add a strict warning-budget row only if the target missing-prototype category
  reaches zero.
- Update warning documentation.

Success criteria:

- `src/samplosf/src/dtsamples/tunecheck.c` has zero strict
  `-Wmissing-prototypes` warnings for `MakeTunerParams`.
- Default behavior gates pass.

Rules:

- Do not change generated tuner strings.
- Do not change public APIs, exported symbols, audio output, or sample command
  behavior.
- Do not mix unused-variable, qualifier, or pointer-sign cleanup into this
  phase.

## Phase 5: Tunecheck Unused-Variable Cleanup

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Clean one low-risk strict unused-variable warning category in the tunecheck
  sample tool.
- Preserve tunecheck behavior and deterministic DECtalk baselines.

Required work:

- Remove only unused local variables in
  `src/samplosf/src/dtsamples/tunecheck.c`.
- Do not change callback logic, buffer processing, audio file generation,
  tuner-string generation, or command-line parsing.
- Add a strict warning-budget row only if the file reaches zero
  `-Wunused-variable` warnings.
- Update warning documentation.

Success criteria:

- `src/samplosf/src/dtsamples/tunecheck.c` has zero strict
  `-Wunused-variable` warnings.
- Default and CMake final gates pass later in Phase 6.

Rules:

- Do not change live audio behavior, callback runtime behavior, sample command
  behavior, generated dictionaries, public APIs, or exported symbols.
- Do not mix qualifier, pointer-sign, format, or timing cleanup into this
  phase.

## Phase 6: Final Readiness Review, PR, And Merge

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Record the final readiness state for this plan.
- Publish and merge the completed work.

Required work:

- Run the final Autotools verification gate.
- Run the final CMake subset verification gate.
- Run the final audio-option matrix.
- Record final warning counts, warning budgets, public headers, exported
  symbols, dictionaries, user dictionaries, public API smoke, callback smoke,
  deterministic audio, CMake status, platform-wrapper status, and deferred
  risks.
- Update `docs/modernization/READINESS_REVIEW.md`.
- Update this plan with the final Implementation Summary.
- Commit final documentation and any remaining intended files.
- Create a pull request with an appropriate body.
- Merge the pull request once required checks and repository policy allow it.

Success criteria:

- Final Autotools verification passes.
- Final CMake subset verification passes.
- Warning budgets pass.
- No unexplained behavior, symbol, header, dictionary, manifest, API, callback,
  phoneme, text, timing, or audio deltas remain.
- The PR is created and merged, or a repository-policy blocker is documented
  with exact status.

Rules:

- Do not claim coverage for live audio hardware, queue timing, backend device
  selection, non-current targets, or phoneme/text output unless this plan
  actually adds and runs those checks.
- Do not merge if required checks fail.

## Definition Of Done For Each Phase

Each completed phase summary must state:

- files changed or created;
- what was implemented;
- verification commands and results;
- whether warning counts changed;
- whether public exports changed;
- whether dictionaries changed;
- whether golden audio changed;
- behavior risk and remaining limitations.

## Current Immediate Next Step

Run Phase 1 against the merged PR #6 baseline. Extra-high reasoning is assumed
for all phases, so no reasoning-level prompt is required.
