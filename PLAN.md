# PLAN.md

## Objective

Continue DECtalk modernization after the completed follow-on modernization
plan, while preserving current Linux behavior.

This plan moves from verification expansion into carefully bounded higher-risk
objectives: CMake packaging parity, broader deterministic behavior coverage,
focused warning-budget expansion, API-boundary cleanup, and platform-wrapper
parity work. The plan does not approve speech output changes, public API
changes, dictionary format changes, install-layout changes, live-audio behavior
changes, or historical target deletion.

## Guiding Principle

Behavior preservation is more important than cleanup.

Every risky change must have a matching verification gate before implementation.
If a gate cannot detect the relevant class of regression, stop and improve the
gate or defer the change.

## Current Intended Target

The current supported target for modernization work is Linux.

Historical targets such as Windows, macOS, IOS, OSF/Tru64, VxWorks, MS-DOS,
Windows CE, Solaris/SPARC, ARM7, MIPS, old PowerPC Mac, iPAQ Linux, and
Emscripten may be inventoried, documented, or kept behind explicit legacy
options. They must not be deleted or broadly rewritten without explicit
approval. macOS is a non-current target.

## Existing Knowledge Base

The previous plans are complete and summarized by:

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
- `tools/baseline/`
- `tests/golden/`
- `src/platform/`

Known follow-on plan final readiness facts:

- PR #2 merged the completed follow-on plan into `develop` as merge commit
  `23e90dd74355bf635521e9374cca7b031c0a1113`.
- Autotools remains the authoritative Linux build path.
- `tools/baseline/verify_current.sh --run-dir baseline-runs/next-phase11-readiness --expected tests/golden`
  passed.
- Default warning-line count: 1,793.
- Strict warning-line count: 29,779.
- Parser-visible default warnings: 1,772.
- Warning budget status was `ok`.
- Current warning budgets enforce:
  `src/dapi/src/dic/dic_comm.c` `-Wformat=` maximum `0`, and
  `src/samplosf/src/dtsamples/mfg_load.c` `-Wold-style-definition` maximum
  `0`.
- US English one-shot golden WAVs matched exactly for speakers 0 through 8.
- Expanded deterministic US audio suites matched exactly for speakers 0 through
  8: `us_abbreviations`, `us_commands_markup`, and
  `us_punctuation_numbers`.
- Exported symbols matched committed symbol baselines in local exact checks.
- Main dictionaries and the US user-dictionary fixture matched committed
  baselines.
- Public header allowlists matched.
- Public API smoke coverage exists for installed headers/libraries, no-audio
  startup, US English speaker 0, WAV output, sync, close, and shutdown.
- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next-phase11-cmake --expected tests/golden`
  passed.
- CMake remains side-by-side only and is not ready to replace Autotools.
- CMake packaging remains incomplete. Phase 10 recorded 589 Autotools paths,
  530 CMake staged paths, 59 missing Autotools paths, and 0 extra CMake paths.
- Hosted CI uses stable symbol type/name and manifest path/type checks where
  runner-specific addresses, binary metadata, or optional runner-built tools
  vary. Local readiness gates keep exact comparisons by default.
- `src/platform/` remains isolated scaffolding. Phase 8 explicitly deferred
  runtime wrapper wiring because current wrappers are not drop-in replacements
  for legacy runtime semantics.
- Live audio hardware behavior, callback timing, backend device selection,
  non-current platform builds, and full API conformance were not tested.
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
- After completing the requested plan scope, create a pull request with an
  appropriate body summarizing changes, verification, behavior risk, and
  limitations, then automatically merge that PR once required checks and
  repository policy allow it.
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
- adding non-US, phoneme, public API, or timing baselines that may expose
  previously untracked differences
- changing CMake source membership, generated dictionary flow, staged packaging
  layout, or multi-language `libtts.so` behavior
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
- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/<phase>-cmake --symbol-mode name-type --expected tests/golden`
- `tools/baseline/capture_dist_manifest.sh --format metadata-hash --out baseline-runs/<phase>/dist-manifest-detailed.txt`
- `tools/baseline/compare_manifest.sh --expected tests/golden/dist-manifest-detailed.txt --actual baseline-runs/<phase>/dist-manifest-detailed.txt --out baseline-runs/<phase>/dist-manifest-detailed.diff`
- `tools/baseline/check_public_headers.sh --out baseline-runs/<phase>/public-headers --expected tests/golden/public-headers`
- `tools/baseline/check_api_smoke.sh --out baseline-runs/<phase>/api-smoke`
- `tools/baseline/summarize_warnings.py --log baseline-runs/<phase>/build/build.log --out-dir baseline-runs/<phase>/warnings-default`
- `tools/baseline/check_warning_budgets.py --warnings baseline-runs/<phase>/warnings-default/warnings.tsv --budget tests/golden/warnings/default-cleaned.tsv --out baseline-runs/<phase>/warning-budget.tsv`

Do not claim behavior preservation unless the relevant checks were run.

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
- Capture current warning counts and compare them to the follow-on final
  readiness values.
- Document the refreshed baseline in `docs/modernization/READINESS_REVIEW.md`
  or a new dated note.

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

- Confirmed local `develop` matches merged `origin/develop` at
  `39f8e89039b0c804d4a2956b0ca4ed1cc1ca5531`.
- Updated `docs/modernization/READINESS_REVIEW.md` with the next plan Phase 1
  post-merge baseline refresh.
- Files changed: `docs/modernization/READINESS_REVIEW.md` and `PLAN.md`.
- Verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next2-phase1-post-merge --expected tests/golden`.
- Verification results: default warning-line count 1,793; strict warning-line
  count 29,779; parser-visible default warnings 1,771; warning budget status
  `ok`; public headers matched; public API smoke output matched the committed
  speaker 0 golden WAV exactly; exported symbols matched; generated
  dictionaries matched; generated US user-dictionary fixture output matched;
  one-shot US English golden WAVs matched for speakers 0 through 8; expanded US
  audio suites matched for speakers 0 through 8; detailed Autotools dist
  manifest matched the committed baseline and the basic manifest contained 589
  entries.
- Additional verification run:
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next2-phase1-post-merge-cmake --expected tests/golden`.
- Additional verification results: CMake configured and built
  `dectalk_cmake_stage`; `compile_commands.json` had 3,127 lines; generated
  dictionaries matched; one-shot and expanded US audio matched; `libtts.so`
  exported symbols matched exactly; language-library exported symbol name/type
  sets matched; detailed CMake staged manifest had 1,041 lines.
- Warning counts changed only by observation: headline default and strict
  warning-line counts matched the follow-on final readiness review; the
  parser-visible default warning count decreased from 1,772 to 1,771.
- Public exports did not change according to the committed symbol comparisons.
- Dictionaries did not change according to the committed dictionary
  comparisons.
- Golden audio did not change according to the committed one-shot and expanded
  deterministic WAV comparisons.
- Behavior risk level: low. This phase changed documentation only after
  verification passed.
- Known limitations: CMake remains side-by-side and packaging parity is still
  pending; live audio hardware, callback timing, backend device selection,
  non-current platform builds, and full API conformance were not tested.

## Phase 2: Higher-Risk Work Selection And Gate Map

Status: completed

Reasoning checkpoint: high.

Goals:

- Select the exact higher-risk work items this plan will attempt.
- Map each work item to a behavior gate before source changes begin.
- Prevent mixed cleanup by separating CMake parity, API cleanup, warning
  cleanup, platform parity, and macro quarantine work.

Required work:

- Review `docs/modernization/WARNING_INVENTORY.md`,
  `docs/modernization/RISK_AREAS.md`, `docs/modernization/CMAKE_OVERVIEW.md`,
  `docs/modernization/PUBLIC_API_AUDIT.md`, and
  `docs/modernization/PLATFORM_WRAPPER_DECISION.md`.
- Produce a short plan note under `docs/modernization/` identifying:
  - files or modules targeted by this plan
  - risk level for each target
  - required verification commands
  - explicitly deferred areas
- Do not edit source code in this phase.

Success criteria:

- The target list is concrete enough that later phases can stay scoped.
- Every target has a matching gate.
- Deferred work is explicit.

Rules:

- If the selected work would require changing speech output, public APIs,
  exported symbols, dictionary behavior, live-audio behavior, or threading
  semantics, stop and ask for explicit approval before adding it to the plan.

Implementation Summary:

- Added `docs/modernization/NEXT_PLAN_GATE_MAP.md` to map the plan's
  higher-risk targets to required behavior gates before source changes begin.
- Updated `docs/modernization/README.md` to include the new gate-map document.
- Files changed: `docs/modernization/NEXT_PLAN_GATE_MAP.md`,
  `docs/modernization/README.md`, and `PLAN.md`.
- The target map covers deterministic phoneme/text-mode feasibility, public API
  smoke expansion, CMake staged packaging parity, CMake parity decision,
  low-risk warning budget expansion, API-boundary warning cleanup, platform
  wrapper parity, runtime wrapper pilot decision, macro/historical-target
  quarantine audit, and final readiness review.
- Deferred areas remain explicit: speech, phoneme, parser, LTS, VTM, HLSYN,
  timing logic, public headers, ABI, exported symbols, dictionary behavior,
  live-audio routing, callback timing, queue/pipe behavior, `opthread.c`,
  `linux_audio.c`, runtime wrapper wiring, CMake promotion, and historical
  target deletion.
- Verification run: `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change; no warning-producing source was modified.
- Public exports did not change; no compiled source or public header was
  modified.
- Dictionaries did not change.
- Golden audio did not change.
- Behavior risk level: low. This phase changed planning documentation only.
- Known limitations: this phase selected and mapped gates but did not run
  source-changing target work; future phases must still run their specific
  verification gates before claiming behavior preservation.

## Phase 3: Deterministic Phoneme And Text-Mode Baseline Feasibility

Status: completed

Reasoning checkpoint: high for feasibility; extra-high before accepting any new
golden baseline.

Goals:

- Determine whether deterministic public-facing phoneme or text-mode output can
  be captured without changing behavior.
- Improve regression coverage before API-boundary and parser-adjacent cleanup.

Required work:

- Inventory existing tools and public APIs that can emit phoneme or text-mode
  output deterministically without live audio hardware.
- Prefer public-facing output over internal captures unless internal capture is
  needed and approved.
- Create scripts only if the output source is deterministic and scoped.
- If feasible, propose exact fixture text, speakers/language, expected files,
  and comparison method before accepting baselines.
- If not feasible, document the blocker and defer without source changes.

Success criteria:

- Feasibility is documented.
- Any accepted baseline has exact capture and compare scripts.
- Existing audio, dictionary, symbol, public header, API smoke, and warning
  budget gates still pass.

Rules:

- Do not accept new golden files without extra-high approval.
- Do not change parser, phoneme, LTS, or synthesis code in this phase.

Implementation Summary:

- Added `docs/modernization/PHONEME_BASELINE_FEASIBILITY.md` to inventory
  deterministic public-facing phoneme and text-mode capture paths.
- Updated `docs/modernization/README.md` to include the new feasibility note.
- Files changed: `docs/modernization/PHONEME_BASELINE_FEASIBILITY.md`,
  `docs/modernization/README.md`, and `PLAN.md`.
- Inventoried `TextToSpeechConvertToPhonemes`,
  `TextToSpeechOpenLogFile(..., LOG_PHONEMES)`, Linux `dist/say`,
  `[:phoneme on]` command text, and in-memory API phoneme arrays.
- Temporary probes under ignored `baseline-runs/next2-phase3-feasibility/`
  showed that `TextToSpeechConvertToPhonemes` compiles against the staged
  public API but crashes at runtime, while `LOG_PHONEMES` log-file mode
  compiles but returns `MMSYSERR_ERROR` in the no-audio staged Linux setup.
- `dist/say -h` did not expose the Windows-style `-lp` phoneme logging option;
  inline `[:log phonemes on]` with `dist/say -fo` completed but did not create
  a separate phoneme log artifact.
- No new golden files were accepted. Phoneme/text-mode baseline acceptance is
  deferred until an extra-high checkpoint proves a reliable public capture path
  or explicitly approves API/runtime repair work.
- Verification run: `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Warning counts did not change; no warning-producing source was modified.
- Public exports did not change; no compiled source or public header was
  modified.
- Dictionaries did not change.
- Golden audio did not change.
- Behavior risk level: low. This phase changed documentation only and recorded
  exploratory ignored artifacts outside the tracked tree.
- Known limitations: no phoneme baseline now exists; future phoneme or
  text-mode baseline work remains extra-high because current public capture
  candidates are unstable or unavailable in the Linux no-audio baseline setup.

## Phase 4: Public API Smoke Matrix Expansion

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Expand public API smoke coverage before API-boundary cleanup.
- Keep coverage deterministic and no-audio by default.

Required work:

- Extend or add API smoke tooling to cover a broader matrix of stable public
  API calls without changing API signatures.
- Candidate coverage includes startup/shutdown variants, language enumeration,
  get/set rate, get/set speaker, get language, version/caps/features calls,
  WAV output open/close, sync, and error-safe shutdown paths.
- Keep tests focused on return codes, stable scalar outputs, and deterministic
  WAV output where applicable.
- Document unsupported or unstable API calls instead of forcing brittle tests.

Success criteria:

- API smoke coverage builds against installed headers and libraries.
- New checks run from `dist/` so dictionary/config loading matches installed
  behavior.
- Public headers and exported symbols remain unchanged.
- Existing audio and dictionary baselines still pass.

Rules:

- Do not modify public headers.
- Do not change exported symbol names or public API signatures.
- Do not claim full API conformance.

Implementation Summary:

- Expanded `tools/baseline/api_smoke.c` from a narrow speak-to-WAV smoke test
  into a deterministic public API smoke matrix while preserving the existing
  byte-for-byte US English speaker 0 WAV comparison.
- Updated `tools/baseline/check_api_smoke.sh` to describe scalar API coverage
  and record the API run log in its summary.
- Updated `docs/modernization/PUBLIC_API_AUDIT.md` with the current smoke
  matrix coverage and explicit unsupported areas.
- Files changed: `tools/baseline/api_smoke.c`,
  `tools/baseline/check_api_smoke.sh`,
  `docs/modernization/PUBLIC_API_AUDIT.md`, and `PLAN.md`.
- New smoke coverage includes `TextToSpeechShutdown(NULL)`,
  `TextToSpeechEnumLangs`, `TextToSpeechVersion`,
  `TextToSpeechGetFeatures`, `TextToSpeechVersionEx`,
  `TextToSpeechStartLang`, `TextToSpeechSelectLang`,
  `TextToSpeechGetCaps`, no-audio `TextToSpeechStartup`, get/set/error cases
  for rate, speaker, language, and main volume, and
  `TextToSpeechGetStatus(INPUT_CHARACTER_COUNT)`.
- The smoke test restores rate, speaker, and volume state before generating the
  accepted speaker 0 WAV artifact.
- Verification run:
  `tools/baseline/check_api_smoke.sh --out baseline-runs/next2-phase4-api-smoke-probe`.
- Verification result: API smoke matrix passed and the generated WAV compared
  exactly with `tests/golden/audio/us/speaker_0.wav`.
- Full verification run:
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next2-phase4-api-smoke --expected tests/golden`.
- Full verification results: default warning-line count 1,793; strict
  warning-line count 29,778; parser-visible default warnings 1,772; warning
  budget status `ok`; public headers matched; expanded API smoke output
  matched the committed speaker 0 golden WAV exactly; exported symbols matched;
  generated dictionaries matched; generated US user-dictionary fixture output
  matched; one-shot US English golden WAVs matched for speakers 0 through 8;
  expanded US audio suites matched for speakers 0 through 8; detailed
  Autotools dist manifest matched the committed baseline.
- Additional verification run:
  `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`.
- Public headers did not change.
- Exported symbols did not change.
- Dictionaries did not change.
- Golden audio did not change.
- Behavior risk level: low to medium. This phase changed only verification
  tooling and documentation, but the tool now exercises additional public API
  behavior before future API-boundary cleanup.
- Known limitations: the smoke matrix still does not test live audio hardware,
  callbacks, in-memory output, phoneme/log capture, non-US speech output,
  thread lifecycle, queue/pipe timing, or full API conformance.

## Phase 5: CMake Packaging Gap Closure

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Close the remaining CMake staged-layout gap without promoting CMake.
- Preserve Autotools as the authoritative Linux build path.

Required work:

- Reconfirm the current Autotools-vs-CMake staged-layout gap.
- Address remaining CMake packaging omissions only when the Autotools source is
  explicit and behavior-neutral.
- Candidate gaps from the previous plan are unbuilt `aclock` and `dtmemory`,
  generated sample text files under `src/DECtalk/dtsamples/`,
  helper/user-dictionary tools under `tools/`, and staged `/usr/bin` symlinks.
- Keep CMake changes side-by-side and Linux-scoped.
- Update `docs/modernization/CMAKE_OVERVIEW.md` and packaging documentation.

Success criteria:

- CMake configures and builds `dectalk_cmake_stage`.
- CMake dictionaries, one-shot US audio, expanded US audio suites, `libtts.so`
  symbols, language-library symbol name/type sets, and `compile_commands.json`
  still pass the relevant verifier.
- Autotools accepted baselines still pass.
- Remaining CMake packaging differences are either eliminated or explicitly
  documented.

Rules:

- Do not promote CMake to primary.
- Do not remove or weaken Autotools.
- Do not change generated dictionary flow unless explicitly required and
  verified.

Implementation Summary:

- Changed `CMakeLists.txt` only for side-by-side CMake packaging:
  - added CMake targets for `aclock`, `dtmemory`, `dump_vdf`, `mfg_load`,
    `say_demo_*`, `tunecheck_*`, and `udic_*`;
  - added generated sample text staging for `birthday.txt`, `demo.txt`,
    `startup.txt`, and `noglass.txt`;
  - added staged `/usr/bin` symlinks to match the Autotools basic path/type
    layout.
- Updated `docs/modernization/CMAKE_OVERVIEW.md` and
  `docs/modernization/PACKAGING_LAYOUT.md` to record the new CMake staged
  layout status.
- Reconfirmed the pre-change gap: Autotools basic manifest had 589 entries and
  CMake had 530 entries, with 59 missing CMake paths and no CMake-only paths.
- Verified the updated CMake stage with
  `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next2-phase5-cmake-after --expected tests/golden`.
  CMake dictionaries matched, one-shot US audio matched for speakers 0 through
  8, expanded US audio suites matched for speakers 0 through 8, `libtts.so`
  symbols matched, language-library symbol name/type sets matched, and
  `compile_commands.json` was present.
- Compared the updated CMake basic staged manifest against the latest accepted
  Autotools basic staged manifest: 589 Autotools entries, 589 CMake entries,
  no missing paths, and no extra paths.
- Compared detailed metadata-hash manifests and documented the remaining
  non-path differences: both detailed manifests have 1,126 lines, but
  CMake-built binaries have different sizes and hashes, and the
  `doc/DECtalk/html` directory metadata differs.
- Re-ran the authoritative Autotools verifier with
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next2-phase5-autotools --expected tests/golden`.
  Default warning-line count was 1,793, strict warning-line count was 29,779,
  parser-visible default warnings were 1,772, and the warning budget passed.
  Public headers, exported symbols, detailed Autotools manifest, dictionaries,
  user dictionaries, API smoke WAV, one-shot US audio, and expanded US audio
  suites all matched their accepted baselines.
- Ran `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
  successfully.
- Behavior risk level: medium. This phase added build-system packaging targets
  and staging rules only; it did not change DECtalk C sources, public headers,
  generated dictionary inputs, runtime selection, audio backend behavior, or the
  authoritative Autotools build path.

## Phase 6: CMake Parity Decision Checkpoint

Status: completed

Reasoning checkpoint: extra-high.

Goals:

- Decide whether CMake packaging has enough parity for a later promotion plan.
- Avoid accidental promotion during this plan.

Required work:

- Compare Autotools and CMake staged manifests after Phase 5.
- Record exact remaining path/type, metadata, symbol, and artifact differences.
- Update `docs/modernization/CMAKE_OVERVIEW.md` with a promote/defer
  recommendation.

Success criteria:

- Recommendation is explicit: promote later, defer, or continue side-by-side.
- Any remaining differences are listed and risk-classified.
- No build system is removed.

Rules:

- Promotion is a separate future decision, not automatic in this phase.

Implementation Summary:

- Updated `docs/modernization/CMAKE_OVERVIEW.md` with an explicit Phase 6
  recommendation: continue side-by-side and defer CMake promotion.
- Rechecked the Phase 5 basic staged manifest comparison with
  `tools/baseline/compare_manifest.sh --expected baseline-runs/next2-phase4-api-smoke/dist-manifest.txt --actual baseline-runs/next2-phase5-cmake-after/dist-manifest-basic.txt --out baseline-runs/next2-phase5-cmake-after/dist-manifest-basic-phase6.diff`.
  Result: exact path/type parity, 589 Autotools entries, 589 CMake entries, no
  missing CMake paths, and no CMake-only paths.
- Rechecked the detailed metadata-hash manifest comparison with
  `tools/baseline/compare_manifest.sh --expected tests/golden/dist-manifest-detailed.txt --actual baseline-runs/next2-phase5-cmake-after/dist-manifest-detailed.txt --out baseline-runs/next2-phase5-cmake-after/dist-manifest-detailed-phase6.diff`.
  Result: manifests still differ. Both detailed manifests have 1,126 lines, but
  CMake-built binaries have different sizes and hashes, and
  `doc/DECtalk/html` directory metadata differs.
- Recorded symbol parity status: `libtts.so` matches the committed full symbol
  baseline exactly; language-library exported symbol name/type sets match for
  all six language libraries; full language-library symbol captures still differ
  for `libtts_us.so`, `libtts_uk.so`, `libtts_sp.so`, `libtts_la.so`,
  `libtts_gr.so`, and `libtts_fr.so`, with matching line counts.
- Risk classification: low for path/type layout, medium for detailed
  metadata/hash release equivalence, low to medium for language-library full
  symbol address/order differences, and medium for unimplemented CMake live
  audio backend option parity.
- No build system was removed or promoted.

## Phase 7: Low-Risk Warning Budget Expansion

Status: completed

Reasoning checkpoint: high for low-risk auxiliary files; extra-high if cleanup
touches API, synthesis, phoneme, LTS, VTM, HLSYN, audio, threading, or pointer
conversion behavior.

Goals:

- Reduce warning debt in low-risk auxiliary code.
- Expand warning budgets only after focused cleanup passes verification.

Required work:

- Select one narrow warning category and one small ownership area from the
  low-risk first wave.
- Preferred candidate areas are auxiliary tools or samples outside runtime
  synthesis paths, such as `src/dtalkml/src/`, `src/licunix/src/`,
  `src/udicunix/src/`, or private command/tool files under `src/dapi/src/cmd/`.
- Preferred warning categories are missing prototypes in private `.c` files,
  old-style definitions in private tools, unused parameters with explicit
  `(void)` markers, missing standard includes, and obvious format fixes.
- Add or update warning-budget entries only for categories reduced to zero.

Success criteria:

- Cleanup is small and locally obvious.
- Autotools accepted baselines pass.
- Warning budget remains `ok`.
- New budget entries prevent the cleaned warnings from returning.

Rules:

- Do not mix warning categories.
- Do not touch public headers or exported symbols.
- Do not perform pointer signedness, qualifier, callback, volatile,
  concurrency, structure layout, or size-truncation cleanup in this phase.

Implementation Summary:

- Changed `src/licunix/src/liceninc.c` only in low-risk auxiliary licensing
  tool code by increasing the local `line` buffer from 1,000 to 1,010 bytes.
  This keeps the existing `sprintf(line, "licenses:%s\n", encrypt)` flow while
  allowing room for the 9-byte prefix, the existing 1,000-byte encrypted-value
  buffer, newline, and terminator.
- Added a zero-count warning-budget ratchet for
  `src/licunix/src/liceninc.c` `-Wformat-overflow=` in
  `tests/golden/warnings/default-cleaned.tsv`.
- Verified with
  `tools/baseline/verify_current.sh --run-dir baseline-runs/next2-phase7-warning --expected tests/golden`.
  Default warning-line count was 1,790, strict warning-line count was 29,776,
  and parser-visible default warnings were 1,769.
- Confirmed `-Wformat-overflow=` is absent from
  `baseline-runs/next2-phase7-warning/warnings-default/by-flag.tsv`, removing
  the three previous `src/licunix/src/liceninc.c` warnings.
- Warning budget passed with the new row:
  `src/licunix/src/liceninc.c`, `-Wformat-overflow=`, actual count 0.
- Public headers, exported symbols, detailed Autotools manifest, dictionaries,
  user dictionaries, API smoke WAV, one-shot US audio, and expanded US audio
  suites all matched their accepted baselines.
- Ran `git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'`
  successfully.
- Behavior risk level: low. This phase touched only an auxiliary license
  maintenance tool and a warning-budget file; it did not change public headers,
  exported symbols, runtime synthesis, dictionaries, audio backend behavior, or
  the threading model.

## Phase 8: API-Boundary Warning Cleanup

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Use the expanded API smoke coverage to address one narrow API-boundary warning
  class.
- Preserve public API behavior and exported symbols.

Required work:

- Select one warning class near API implementation files only after Phase 4
  coverage exists.
- Candidate files include `src/dapi/src/api/init.c` and narrowly scoped private
  implementation code near `ttsapi.c`; public headers are excluded unless a
  later explicit approval says otherwise.
- Prefer private prototypes, missing includes, or internal declaration cleanup.
- Avoid callback signature, structure layout, calling convention, pointer
  conversion, and threading lifecycle changes unless separately approved.

Success criteria:

- Public headers match allowlists.
- Exported symbols match committed baselines.
- API smoke matrix passes.
- Audio and dictionary baselines pass.
- Warning counts and any budget changes are documented.

Rules:

- Do not change public API signatures.
- Do not change exported symbol names or calling conventions.
- Stop on any API smoke, symbol, header, audio, or dictionary delta.

## Phase 9: Platform Wrapper Parity Harness

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Add parity tests or documentation for `src/platform` wrappers before any
  runtime wiring is reconsidered.
- Keep runtime behavior untouched.

Required work:

- Identify legacy runtime semantics that blocked Phase 9 of the previous plan:
  stack size, priority, timeout handling, handle ownership, scheduler-yield,
  lightweight locks, audio routing, callback timing, and buffer behavior.
- Add tests or documentation only for wrapper semantics that can be verified
  without changing runtime code.
- Update `docs/modernization/PLATFORM_WRAPPER_DECISION.md`.

Success criteria:

- `src/platform` smoke/parity coverage improves or the blocker is documented.
- No runtime files are wired to wrappers.
- Existing Autotools and CMake gates still pass when relevant.

Rules:

- Do not change `src/dapi/src/nt/opthread.c`.
- Do not change `src/dapi/src/nt/linux_audio.c`.
- Do not route runtime audio, threading, mutex, event, filesystem, or time code
  through `src/platform` in this phase.

## Phase 10: Runtime Wrapper Pilot Decision

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Decide whether any runtime wrapper pilot is safe after Phase 9.
- Prefer deferral unless parity evidence is strong.

Required work:

- Review Phase 9 evidence.
- If no safe candidate exists, mark this phase completed as deferred and record
  why.
- If a safe candidate exists, propose the exact file, wrapper, behavior gate,
  and rollback plan before implementation.

Success criteria:

- Decision is documented.
- Any implementation is tiny, opt-in if possible, and verified by exact
  behavior gates.
- No audio, callback, timing, queue, pipe, or threading lifecycle regression is
  introduced.

Rules:

- Do not implement a runtime wrapper pilot without explicit approval inside this
  phase.
- Do not use live-audio behavior as an unverified assumption.

## Phase 11: Macro And Historical Target Quarantine Audit

Status: not started

Reasoning checkpoint: high for audit; extra-high before changing any macro or
historical-target branch.

Goals:

- Improve macro and historical-target documentation without deleting preserved
  code.
- Prepare future cleanup by separating current Linux defines from historical,
  product, language, and sound-critical defines.

Required work:

- Review `docs/modernization/MACRO_INVENTORY.md` and
  `docs/modernization/HISTORICAL_TARGETS.md`.
- Audit any macro groups that block CMake parity, warning cleanup, or platform
  wrapper work.
- Update documentation with classifications and proposed future quarantine
  options.

Success criteria:

- Macro classifications are clearer.
- Current Linux behavior is unchanged.
- No historical target code is deleted.

Rules:

- Do not simplify sound-critical or language-selection macros.
- Do not remove historical target source branches.
- Build-system opt-in quarantine requires extra-high approval.

## Phase 12: Final Readiness Review For This Plan

Status: not started

Reasoning checkpoint: extra-high.

Goals:

- Confirm the plan completed without hidden behavior changes.
- Produce a concise state-of-the-codebase report for the next plan.

Required review areas:

- Linux build reproducibility.
- Hosted and local CI behavior.
- Warning counts and budgets.
- Public headers, API smoke coverage, and exports.
- Dictionary generation and user-dictionary fixture.
- One-shot and expanded deterministic golden audio.
- Any phoneme or text-mode baselines added by this plan.
- CMake status and packaging parity.
- Platform abstraction status.
- Historical target and macro quarantine status.
- Known risks and deferred work.

Success criteria:

- `docs/modernization/READINESS_REVIEW.md` is updated or a new dated readiness
  review exists.
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
