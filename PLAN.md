# PLAN.md

## Objective

Continue DECtalk modernization after PR #5 while preserving current Linux
behavior.

This plan is the next higher-risk modernization pass. It deliberately keeps
extra-high reasoning active for every phase, so there are no reasoning-change
prompts and no special phase sequencing for reasoning-level transitions.

The plan accelerates the project by pursuing the remaining blockers in one
continuous sequence:

- deterministic phoneme/text feasibility
- callback and queue-adjacent public API coverage
- non-US deterministic audio baseline expansion if stable
- a focused API/kernel warning cleanup wave
- CMake detailed parity investigation and audio-option build matrix evidence
- isolated platform-wrapper adapter scaffolding without default runtime wiring
- final readiness review, pull request, and merge

This plan does not approve uncontrolled speech output changes, phoneme output
changes, dictionary behavior changes, public API changes, install-layout
changes, live-audio behavior changes, threading behavior changes, historical
target deletion, or default runtime wrapper replacement.

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

## Current Starting Facts After PR #5

- PR #5, `Complete accelerated modernization plan`, was merged into `develop`
  as merge commit `05d4dd7b03417f1c56052c911714c9773847885c`.
- Local `develop` is synchronized with `origin/develop`.
- Autotools remains the authoritative Linux build path.
- The final Autotools gate for PR #5 passed:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next3-phase13-final --expected tests/golden`.
- Final PR #5 Autotools warning counts were: default warning-line count
  `1,778`, strict warning-line count `29,665`, parser-visible default warnings
  `1,756`, and parser-visible strict warnings `29,643`.
- The warning budget passed and currently ratchets cleaned warning classes for:
  `src/dapi/src/dic/dic_comm.c`,
  `src/samplosf/src/dtsamples/mfg_load.c`,
  `src/licunix/src/liceninc.c`,
  `src/dapi/src/api/ttsapi.c`, and
  `src/dapi/src/kernel/services.c`.
- Public headers, exported symbols, generated dictionaries, the US
  user-dictionary fixture, the public API smoke WAV, one-shot US English WAVs,
  and expanded deterministic US English audio suites matched accepted
  baselines.
- The final CMake subset gate for PR #5 passed:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase13-final-cmake --expected tests/golden`.
- Final PR #5 CMake evidence included `compile_commands.json` with `3,307`
  lines, generated dictionaries matching baselines, US one-shot and expanded
  audio matching baselines, exact `libtts.so` exported-symbol parity,
  language-library symbol name/type parity, passing `dt_platform_smoke`, and
  passing `opthread_smoke`.
- CMake path/type staged manifest parity matched Autotools exactly with `589`
  entries on each side.
- CMake detailed metadata-hash parity is still not accepted. The CMake and
  Autotools detailed manifests both have `1,126` entries, but CMake-built
  binary sizes/hashes and `doc/DECtalk/html` directory metadata still differ.
- CMake models `DISABLE_AUDIO`, `USE_ALSA`, and `USE_PULSEAUDIO` as
  side-by-side macro options, but live-audio probing, backend linkage, device
  selection, callbacks, queues, timing, and live hardware behavior are not
  certified.
- `src/platform/` remains isolated scaffolding. Runtime wrapper wiring is still
  deferred because legacy `OP_*`, audio, callback, queue, and timing semantics
  are not proven.
- No deterministic phoneme or text-mode golden baseline has been accepted.
- Non-US deterministic audio baselines have not been accepted.
- High-risk warning debt remains in synthesis, phoneme, LTS, VTM, HLSYN,
  public API, threading, and audio code.

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
  header, API, phoneme, or timing delta, stop and resolve it before continuing.
- Do not update golden baselines unless a phase explicitly allows it and the
  relevant before/after evidence is captured.
- Do not claim behavior preservation unless the relevant checks were run.

## Extra-High Scope

Extra-high reasoning is already active and required for the whole plan,
including:

- accepting or updating any golden behavior baseline
- adding non-US, phoneme, public API, callback, queue, threading, or timing
  baselines that may expose previously untracked differences
- changing CMake source membership, generated dictionary flow, staged packaging
  layout, audio option modeling, or multi-language `libtts.so` behavior
- changing public headers, public API signatures, calling conventions, exported
  symbols, installed header layout, or ABI-sensitive structures
- changing API implementation near `ttsapi.c`, `tts.h`, `ttsapi.h`, `init.c`,
  callback paths, or multi-language loader behavior
- changing medium-risk warnings involving conversions, pointer qualifiers,
  pointer/integer casts, callback signatures, thread function signatures,
  `volatile`, structure layout, or size truncation
- editing synthesis, phoneme, LTS, VTM, HLSYN, public API, audio, or threading
  code for warning cleanup
- adding or wiring `src/platform` thread, mutex, event, filesystem, time, or
  audio adapters
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
- `tools/baseline/summarize_warnings.py --log baseline-runs/<phase>/build/build-strict-warnings.log --out-dir baseline-runs/<phase>/warnings-strict`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/<phase>/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/<phase>/warning-budget.tsv`

## Phase 1: Post-PR #5 Baseline Refresh

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Confirm the local tree is synchronized with merged `origin/develop`.
- Re-run the accepted Linux baseline from the merged PR #5 branch.
- Establish starting warning, symbol, dictionary, audio, API, manifest, CMake,
  platform-smoke, and OP-smoke state for this plan.

Required work:

- Ensure local `develop` tracks merged `origin/develop`.
- Run the Autotools verification gate against `tests/golden`.
- Run the CMake subset verification gate with default exact local checks.
- Generate default and strict warning parser summaries from the refreshed logs.
- Compare current counts to the PR #5 final readiness values.
- Update `docs/modernization/READINESS_REVIEW.md` with the refreshed starting
  state.

Success criteria:

- Accepted Autotools baselines reproduce.
- Accepted CMake subset baselines reproduce.
- Warning counts and any deltas are recorded.
- No source behavior changes are made.

Rules:

- This phase is a synchronization and verification checkpoint only.
- Do not update golden files in this phase unless repeated captures prove the
  committed post-merge detailed manifest is stale and all other behavior gates
  pass. Any such repair must be documented before advancing.

Implementation Summary:

- Updated `tests/golden/dist-manifest-detailed.txt` after two independent
  post-PR #5 Autotools captures reproduced the same detailed binary hash deltas
  for the language shared libraries and `say_demo_*` tools while all other
  behavior gates passed. The first and second captured detailed manifests
  matched each other exactly, proving the committed post-merge detailed manifest
  was stale rather than unstable.
- Updated `docs/modernization/READINESS_REVIEW.md` with the refreshed
  post-PR #5 baseline evidence, warning counts, and CMake subset status.
- Final Autotools verification passed with public headers, exported symbols,
  generated dictionaries, the US user-dictionary fixture, API smoke output,
  one-shot US English audio, expanded US English audio suites, detailed
  manifest, and warning budget all matching accepted baselines. Recorded counts
  were 1,778 default warning lines, 29,665 strict warning lines, 1,757
  parser-visible default warnings, and 29,642 parser-visible strict warnings.
- Final CMake subset verification passed for dictionary generation,
  deterministic US English audio, expanded audio suites, `libtts.so` exported
  symbols, language-library symbol name/type sets, platform smoke, and OP thread
  smoke. CMake path/type staging matched Autotools with 589 entries each; CMake
  detailed metadata-hash staging still differs from Autotools with 1,126 entries
  each.
- No source, public API, build script, runtime, dictionary-generation, audio, or
  threading behavior was intentionally changed in this phase.

## Phase 2: Risk Backlog And Gate Map Compression

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Convert the remaining deferred modernization areas into a concrete
  implementation queue.
- Keep the next risky phases bounded by verifiable gates.

Required work:

- Review `READINESS_REVIEW.md`, `BASELINE_EXPANSION_PLAN.md`,
  `PHONEME_BASELINE_FEASIBILITY.md`, `PUBLIC_API_AUDIT.md`,
  `API_BOUNDARY_WARNINGS.md`, `CMAKE_OVERVIEW.md`,
  `PACKAGING_LAYOUT.md`, `AUDIO_BACKEND.md`, and
  `PLATFORM_WRAPPER_DECISION.md`.
- Update `docs/modernization/NEXT_PLAN_GATE_MAP.md` for this plan.
- Select initial candidate scopes for phoneme/text capture, callback coverage,
  non-US audio capture, warning cleanup, CMake parity work, and platform
  adapter work.
- Explicitly mark any candidate that needs new tooling before behavior can be
  claimed.

Success criteria:

- Each later implementation phase has an explicit candidate and rollback rule.
- No code, build script, runtime behavior, or golden artifact changes are made.

Rules:

- Documentation-only phase.
- Do not widen later phases beyond what can be verified deterministically.

## Phase 3: Phoneme And Text Output Baseline Pilot

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Determine whether deterministic phoneme or text-mode output can be captured
  and accepted as a golden baseline.
- Prefer public-facing tool or API output over internal memory capture.

Required work:

- Inspect existing command-line tools, API flags, and prior feasibility notes.
- Add a focused capture script under `tools/baseline/` only if the capture path
  is deterministic and does not require live audio.
- Capture fixed US English inputs first.
- If the output is stable across repeated captures, add golden fixtures and
  exact compare scripts.
- If the output is unstable or unavailable, document the blocker and defer
  without accepting a baseline.

Success criteria:

- Either a deterministic phoneme/text baseline is accepted with exact compare
  tooling, or the blocker is documented with reproduction evidence.
- Existing Autotools baseline still passes if scripts, tests, or golden files
  change.
- Public headers, exported symbols, dictionaries, API smoke, one-shot US audio,
  and expanded US audio remain stable.

Rules:

- Do not change parser, phoneme, LTS, synthesis, timing, dictionary, public API,
  or audio generation behavior.
- Golden phoneme/text fixtures may be added only if the phase proves repeatable
  byte-exact output.

## Phase 4: Public API Callback And Queue-Adjacent Smoke Pilot

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Add deterministic coverage for callback and queue-adjacent public API paths
  without live audio hardware.
- Improve safety before any deeper API implementation cleanup.

Required work:

- Extend existing API smoke tooling or add a separate installed-header harness
  under `tools/baseline/`.
- Prefer WAVE-file or no-audio modes that do not open live audio devices.
- Capture stable event ordering and scalar callback data only. Do not assert
  scheduler timing or wall-clock delays.
- Cover only callback paths that are deterministic under repeated runs.
- Update `docs/modernization/PUBLIC_API_AUDIT.md`.

Success criteria:

- The harness builds against installed public headers and libraries.
- Callback output is deterministic across repeated runs.
- Public headers, exported symbols, dictionaries, API smoke WAV, one-shot US
  audio, expanded US audio suites, and warning budgets pass.
- No public API or ABI surface changes.

Rules:

- Do not change callback implementation, queue timing, pipe behavior, thread
  lifecycle, public headers, or exported symbols.
- If callback output is timing-sensitive or unstable, document the blocker and
  defer instead of loosening checks.

## Phase 5: Non-US Deterministic Audio Baseline Expansion

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Expand deterministic WAV coverage beyond US English if the existing language
  outputs are stable.
- Keep the baseline shape simple enough to maintain.

Required work:

- Add or extend capture tooling for UK English, Spanish, German, Latin
  American Spanish, and French using existing staged language libraries.
- Start with one-shot WAV output for speakers 0 through 8.
- Run repeated captures before accepting any new golden files.
- Record sample counts, hashes, and exact comparisons.
- Update `docs/modernization/BASELINE_EXPANSION_PLAN.md` and
  `docs/modernization/READINESS_REVIEW.md`.

Success criteria:

- New non-US WAV baselines are accepted only if repeated captures are
  byte-exact.
- Existing US audio baselines remain byte-exact.
- Dictionaries, user dictionaries, public headers, exported symbols, API smoke,
  and manifests remain stable.

Rules:

- Do not change language selection behavior, voice ROM selection, sample rate,
  default voice, synthesis code, parser behavior, or dictionary behavior.
- If any language output is unstable, defer that language rather than accepting
  fuzzy thresholds.

## Phase 6: Medium-Risk Warning Cleanup Wave

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Use the expanded coverage from earlier phases to reduce one medium-risk
  warning category.
- Keep warning cleanup narrow and behavior-neutral.

Required work:

- Select one file and one warning category from the refreshed strict inventory.
- Prefer API/kernel service stubs, private helpers, missing declarations, or
  unused parameters where behavior can be proven unchanged.
- Avoid synthesis arithmetic, parser rules, dictionary format, public headers,
  callback signatures, thread signatures, `volatile`, structure layout, and
  pointer/integer casts unless this phase is explicitly narrowed to that exact
  issue with matching gates.
- Add a warning-budget row only for a category proven clean.
- Update warning documentation.

Success criteria:

- The selected warning category is reduced or documented as deferred.
- Autotools verification passes.
- Relevant added baseline checks from Phases 3 through 5 still pass.
- Public headers, exported symbols, dictionaries, API smoke, deterministic
  audio, and warning budgets pass.

Rules:

- Do not mix warning categories.
- Do not perform broad formatting.
- Do not change exported symbols or public API signatures.

## Phase 7: API Implementation Warning Pilot

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Evaluate one high-risk API implementation warning cleanup only after stronger
  API and callback coverage exists.
- Preserve ABI and public behavior.

Required work:

- Select one warning candidate from `ttsapi.c`, `init.c`, or adjacent API
  implementation only if local review proves it is narrow.
- Prefer private prototypes, local initialization, or unreachable debug-stub
  hygiene.
- Reject candidates involving callback signatures, structure layout,
  pointer/integer conversions, loader semantics, multi-language dispatch,
  memory ownership, or thread lifecycle unless this phase is revised with a
  dedicated gate.
- Update `docs/modernization/API_BOUNDARY_WARNINGS.md`.

Success criteria:

- Either one narrow API implementation warning is cleaned and fully verified,
  or the phase records why all candidates were deferred.
- Public headers and exported symbols match exactly.
- Expanded API smoke, callback smoke, dictionaries, deterministic audio,
  manifests, and warning budgets pass.

Rules:

- Stop on any API, symbol, header, dictionary, audio, callback, queue, or
  manifest delta that is not explicitly explained and accepted.

## Phase 8: CMake Detailed Parity Closure Attempt

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Reduce or classify CMake detailed metadata-hash differences.
- Keep CMake side-by-side unless all promotion blockers are eliminated and a
  later phase explicitly promotes it.

Required work:

- Compare current Autotools and CMake detailed manifests.
- Classify each difference class: binary size, binary hash, directory
  metadata, symbol address/order, build flags, source membership, generated
  content, or packaging metadata.
- Attempt low-risk CMake adjustments only when they do not alter Autotools or
  default runtime behavior.
- Record whether differences are eliminated, reduced, or accepted as future
  promotion blockers.
- Update `docs/modernization/CMAKE_OVERVIEW.md` and
  `docs/modernization/PACKAGING_LAYOUT.md`.

Success criteria:

- CMake path/type parity still passes.
- CMake deterministic dictionaries, audio, symbols, platform smoke, and OP
  smoke still pass.
- Any CMake build-system change is verified by the CMake subset gate and the
  Autotools gate.

Rules:

- Do not remove Autotools.
- Do not change install layout unless exact path/type and detailed manifest
  evidence supports the change.
- Do not promote CMake in this phase.

## Phase 9: CMake And Autotools Audio Option Build Matrix

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Increase confidence in audio-option configuration without opening live audio
  devices.
- Document which combinations are compile-only, metadata-only, or
  behavior-tested.

Required work:

- Exercise CMake default, `DECTALK_CMAKE_DISABLE_AUDIO=ON`,
  `DECTALK_CMAKE_USE_ALSA=ON`, and `DECTALK_CMAKE_USE_PULSEAUDIO=ON` where
  local dependencies allow.
- Exercise matching Autotools configuration paths if they exist and can be run
  without live audio hardware.
- Capture compile-command, platform-smoke, and selected build metadata.
- Do not open live devices. Do not require ALSA/PulseAudio runtime hardware.
- Update `docs/modernization/AUDIO_BACKEND.md`,
  `docs/modernization/CMAKE_OVERVIEW.md`, and macro documentation.

Success criteria:

- Default audio behavior remains unchanged.
- Compile-only or metadata-only option states are clearly labeled.
- Existing deterministic WAV and API smoke gates still pass for default builds.

Rules:

- Do not change `src/dapi/src/nt/linux_audio.c` runtime behavior.
- Do not change callback timing, queues, device selection, or audio thread
  lifecycle.

## Phase 10: Platform Adapter Scaffold

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Add isolated compatibility scaffolding for future platform-wrapper migration
  without changing default runtime behavior.
- Preserve the exact legacy `OP_*` evidence captured so far.

Required work:

- Review `opthread_smoke` evidence against `src/platform` wrappers.
- If safe, add a documented adapter layer under `src/platform/` that models
  legacy handle ownership, wait return behavior, event semantics, priority
  behavior, and timeout/poll behavior.
- Add or extend CMake smoke coverage for the adapter.
- Keep the adapter out of DECtalk runtime libraries and installed public APIs.
- Update `docs/modernization/PLATFORM_WRAPPER_DECISION.md` and
  `src/platform/README.md`.

Success criteria:

- New source/header files have standard top-of-file documentation.
- Adapter smoke tests pass.
- Existing CMake and Autotools gates still pass.
- No runtime code is routed through the adapter by default.

Rules:

- Do not change `src/dapi/src/nt/opthread.c`.
- Do not change `src/dapi/src/nt/linux_audio.c`.
- Do not change live audio behavior, callback behavior, queue behavior, thread
  lifecycle behavior, or public API behavior.

## Phase 11: Experimental Runtime Wrapper Opt-In Decision

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Decide whether any runtime wrapper wiring can be introduced behind a
  disabled-by-default experimental option.
- Avoid default behavior changes.

Required work:

- Review Phase 10 adapter evidence and current runtime ownership boundaries.
- If safe, add only disabled-by-default build-system scaffolding for an
  experimental wrapper path. The default Autotools and CMake builds must not
  use the wrapper path.
- Prefer compile-only or smoke-only opt-in evidence.
- If no safe opt-in exists, document the blocker and defer.

Success criteria:

- Default runtime artifacts, symbols, dictionaries, and audio remain stable.
- Any experimental option is clearly named, disabled by default, undocumented as
  production-ready, and excluded from installed public APIs.
- Existing default gates pass.

Rules:

- Do not enable wrapper wiring by default.
- Do not change audio, callback, queue, pipe, or thread timing behavior.
- Do not claim runtime wrapper replacement safety unless default and
  experimental gates prove it.

## Phase 12: CMake Promotion Readiness Decision

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Decide whether CMake is ready for any promotion after the detailed parity and
  audio-option matrix work.
- Keep Autotools unless promotion criteria are fully met and explicitly
  recorded.

Required work:

- Review CMake path/type parity, detailed metadata/hash differences,
  language-library symbol evidence, compile-command evidence, audio-option
  matrix evidence, and deterministic behavior checks.
- If blockers remain, record them with exact future gates.
- If blockers are eliminated, draft the smallest promotion step that does not
  remove Autotools or legacy build paths.
- Update CMake, packaging, build overview, and readiness documentation.

Success criteria:

- CMake status is explicit and evidence-based.
- Autotools remains authoritative unless all promotion criteria are met.
- No existing build path is removed.

Rules:

- Do not remove Autotools.
- Do not remove Visual Studio, legacy Visual Studio 6, or devops build paths.
- Do not change install layout without exact manifest evidence.

## Phase 13: Final Readiness Review, PR, And Merge

Status: pending

Reasoning checkpoint: extra-high.

Goals:

- Record the final readiness state for this plan.
- Publish and merge the completed work.

Required work:

- Run the final Autotools verification gate.
- Run the final CMake subset verification gate.
- Run any new baseline comparison gates added by this plan.
- Capture and compare relevant manifests.
- Record final warning counts, warning budgets, public headers, exported
  symbols, dictionaries, user dictionaries, public API smoke, callback smoke,
  deterministic audio, phoneme/text status, CMake status, platform wrapper
  status, and deferred risks.
- Update `docs/modernization/READINESS_REVIEW.md`.
- Update this plan with the final Implementation Summary.
- Commit final documentation and any remaining intended files.
- Create a pull request with an appropriate body.
- Merge the pull request once required checks and repository policy allow it.

Success criteria:

- Final Autotools verification passes.
- Final CMake subset verification passes.
- Warning budgets pass.
- Any new accepted baselines compare exactly.
- No unexplained behavior, symbol, header, dictionary, manifest, API, callback,
  phoneme, text, timing, or audio deltas remain.
- The PR is created and merged, or a repository-policy blocker is documented
  with exact status.

Rules:

- Do not claim coverage for live audio hardware, callbacks, queue timing,
  backend device selection, non-current targets, non-US audio, or phoneme/text
  output unless this plan actually adds and runs those checks.
- Do not merge if required checks fail.

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

Before implementing Phase 1, verify the tree is synchronized with
`origin/develop` after PR #5 and run the Phase 1 baseline refresh. Extra-high
reasoning is assumed for all phases, so no reasoning-level prompt is required.
