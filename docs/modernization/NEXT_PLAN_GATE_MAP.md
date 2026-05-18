# Next High-Risk Plan Gate Map

This note maps the current post-PR #5 modernization plan to concrete work
targets, required evidence, and rollback rules. It is planning documentation
only; it does not approve behavior changes.

## Scope Rules

- Current implementation target: Linux.
- Autotools remains authoritative.
- CMake remains side-by-side until parity is proven and explicitly accepted.
- Extra-high reasoning is active for all phases, so no reasoning-change prompts
  are required unless a regression, ambiguity, or user interruption appears.
- Speech output, phoneme output, dictionary behavior, public API signatures,
  exported symbol names, install layout, live-audio behavior, threading model,
  sample rate, language selection, voice ROM selection, and default voice are
  unchanged unless a phase explicitly approves and verifies a change.
- Historical targets remain preserved and are not deleted.
- New source or header files must include standard top-of-file documentation.
- Each phase must leave a stable, testable checkpoint and must be committed
  before moving to the next phase.

## Phase Gate Map

| Phase | Candidate scope | Required gates | Rollback rule |
| --- | --- | --- | --- |
| 1 | Post-PR #5 baseline refresh | `verify_current.sh`, `verify_cmake_subset.sh`, warning counts, symbols, manifests, dictionaries, public headers, API smoke, one-shot and expanded US audio | Stop on unexplained baseline delta; only refresh a golden manifest after repeated stable captures and passing behavior gates |
| 2 | Compress the knowledge base into this gate map | `git diff --check`; no source, build, runtime, or golden changes | Revert to documentation-only if any implementation decision needs new evidence |
| 3 | Phoneme/text baseline pilot | Public-facing phoneme/text probes, repeated captures, existing Autotools gate if tooling or fixtures change | Do not accept a baseline if `TextToSpeechConvertToPhonemes` crashes, `LOG_PHONEMES` returns an error, no text artifact is produced, or repeated output differs |
| 4 | Callback and queue-adjacent API smoke pilot | Installed-header harness, repeated deterministic callback output, public headers, exact symbols, dictionaries, API WAV, one-shot and expanded US audio, warning budget | Drop any assertion that depends on scheduler timing, wall-clock delay, queue timing, live devices, or callback ordering that is not byte-stable |
| 5 | Non-US deterministic audio expansion | Repeated one-shot WAV captures by language and speaker, existing US audio gates, dictionaries, symbols, headers, API smoke, manifests | Defer a language if output is unstable, if fixed input text is not accepted, or if language selection/voice ROM behavior would need code changes |
| 6 | Medium-risk warning cleanup wave | One file, one warning category, strict warning before/after, Autotools gate, any accepted Phase 3-5 gates, warning budget | Revert the cleanup if it touches public signatures, struct layout, parser/synthesis arithmetic, dictionary format, threading, audio, or produces any behavior delta |
| 7 | API implementation warning pilot | Expanded API/callback gates, exact public headers and exports, dictionaries, deterministic audio, manifests, warning budget | Defer instead of editing if the candidate touches callback signatures, memory ownership, loader dispatch, structure layout, pointer/integer conversions, or thread lifecycle |
| 8 | CMake detailed parity closure attempt | CMake subset gate, Autotools gate for build changes, path/type and detailed manifests, symbols, dictionaries, audio, platform and OP smokes | Revert CMake changes that alter Autotools, default runtime behavior, install path/type parity, or deterministic output |
| 9 | CMake and Autotools audio option build matrix | Default gates plus compile-only or metadata-only option probes; no device opens | Roll back option changes that affect default audio metadata, link unexpected live-audio dependencies, or change device selection/callback/thread behavior |
| 10 | Platform adapter scaffold | Adapter smoke coverage, CMake subset gate, Autotools gate if shared source membership changes, standard docs in new source/header files | Remove the adapter if it is wired into runtime, installed public APIs, `opthread.c`, `linux_audio.c`, or any default DECtalk library |
| 11 | Experimental runtime wrapper opt-in decision | Default Autotools and CMake gates; optional disabled-by-default compile or smoke evidence only | Do not add opt-in wiring if default artifacts change or if queue, pipe, callback, timing, thread, or audio behavior lacks deterministic evidence |
| 12 | CMake promotion readiness decision | Review detailed parity, path/type parity, symbols, compile commands, audio-option matrix, deterministic gates, packaging docs | Keep CMake side-by-side if any promotion blocker remains; do not remove any existing build path |
| 13 | Final readiness, PR, and merge | Final Autotools gate, final CMake subset gate, all accepted new gates, warning budgets, PR checks, merge policy | Stop before merge on any unexplained behavior, symbol, header, dictionary, manifest, API, callback, phoneme, timing, or audio delta |

## Candidate Details

Phase 3 starts from the existing phoneme feasibility evidence. The only
acceptable paths are public-facing and deterministic: a stable
`TextToSpeechConvertToPhonemes` probe, a reliable public log-file mode, or a
command-line artifact that is separate from WAV output. If those paths still
crash, return errors, or fail to produce byte-stable artifacts, the phase should
document the blocker and defer without adding golden files.

Phase 4 may extend `tools/baseline/api_smoke.c` or add a separate
installed-header harness under `tools/baseline/`. Candidate callback coverage is
limited to stable message counts, message type presence, scalar status values,
and file-output/no-audio paths. In-memory phoneme arrays, queue timing, pipe
behavior, live-audio callbacks, and wall-clock ordering remain unsupported until
the harness proves repeatable output.

Phase 5 should start with one-shot WAV baselines for the staged non-US language
libraries: `uk`, `sp`, `gr`, `la`, and `fr`, speakers 0 through 8. Each language
needs a fixed input text, repeated byte-exact captures, recorded hashes, and an
exact comparison path before any fixture is accepted. Existing US English
baselines remain blocking gates.

Phase 6 primary candidate is the `src/dapi/src/api/coop.h`
`-Wdiscarded-qualifiers` cluster only if inspection proves a const-preserving
change with no installed-header or ABI effect. If that is too broad, defer it
and use the strict warning inventory to pick a smaller single-file category.
The `src/dapi/src/kernel/services.c` unused-parameter rows are a lower-risk
fallback, but they should not be mixed with qualifier cleanup.

Phase 7 primary candidates are private local-prototype or local-initialization
warnings in `src/dapi/src/api/ttsapi.c` or `src/dapi/src/api/init.c` only after
Phase 4 coverage is available. Candidate functions such as
`PutIndexMarkInBuffer`, `PutPhonemeInBuffer`, and `WriteAudioToFile` need local
review before editing because they are near callback, phoneme, or file-output
paths. Exported, reserved, callback-facing, loader-adjacent, and memory-owner
warnings stay deferred unless a dedicated gate is added.

Phase 8 focuses on classifying CMake detailed manifest differences that remain
after path/type parity: built-binary size/hash differences, `doc/DECtalk/html`
directory metadata, symbol address/order differences, build flags, source
membership, generated content, and packaging metadata. Low-risk CMake-only
changes are allowed only if default deterministic outputs and Autotools remain
unchanged.

Phase 9 should label every audio-option probe as one of: default behavior
tested, compile-only, metadata-only, unavailable due to local dependencies, or
deferred because it would open live devices. CMake options currently model
`DISABLE_AUDIO`, `USE_ALSA`, and `USE_PULSEAUDIO`; Autotools remains the
authoritative probe and link path.

Phase 10 can add adapter scaffolding only after comparing the existing
`opthread_smoke` evidence with `src/platform` wrapper behavior. Any adapter must
model legacy handle ownership, wait return values, event constants and reset
semantics, priority calls, sleep behavior, timeout/poll behavior, and
lightweight locks without becoming part of default runtime libraries.

Phase 11 is expected to be a decision phase unless Phase 10 creates strong
adapter evidence. Any experimental wrapper path must be disabled by default,
clearly named as experimental, excluded from installed public APIs, and backed
by default-build exact comparisons.

## Tooling Needs

- Phase 3 may need a small capture script only after manual probes show stable
  public output.
- Phase 4 likely needs a new installed-header callback harness or a strictly
  bounded extension to `api_smoke.c`.
- Phase 5 needs language-aware audio capture and comparison support only if
  existing scripts cannot already parameterize language libraries.
- Phase 8 may need a manifest-difference classifier, but manual classification
  is acceptable if the difference set remains small and reproducible.
- Phase 9 may need a build-matrix helper for repeated CMake and Autotools
  option probes.
- Phase 10 needs CMake smoke target wiring for any new adapter source; new
  source and header files must carry standard documentation.

## Deferred Areas

These remain deferred unless a later phase explicitly narrows and verifies
them:

- speech, parser, phoneme, LTS, VTM, HLSYN, and timing logic cleanup
- public header or ABI changes
- exported symbol changes
- dictionary format or lookup behavior changes
- live-audio routing, callback timing, queue, pipe, buffer, or backend behavior
  changes
- `src/dapi/src/nt/opthread.c` runtime behavior changes
- `src/dapi/src/nt/linux_audio.c` runtime behavior changes
- default runtime wiring of `src/platform` wrappers
- CMake promotion to the primary Linux build path
- historical target source deletion

## Gate Policy

Local readiness gates should keep exact comparisons. Hosted CI may use stable
symbol name/type or manifest path/type comparisons only where runner-specific
addresses, binary metadata, or optional runner-built tools differ.

If any target produces an unexplained audio, dictionary, symbol, manifest,
public header, API, callback, phoneme, text, timing, or threading delta, stop
and treat the investigation as an extra-high reasoning checkpoint.
