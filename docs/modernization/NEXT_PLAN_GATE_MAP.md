# Next Warning Cleanup Plan Gate Map

This note maps the current post-PR #6 warning-focused modernization plan to
concrete work targets, required evidence, and rollback rules. It is planning
documentation only; it does not approve behavior changes outside the named
phases.

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
| 1 | Post-PR #6 baseline refresh | `verify_current.sh`, `verify_cmake_subset.sh`, warning counts, symbols, manifests, dictionaries, public headers, API smoke, callback smoke, US and non-US deterministic audio | Stop on unexplained baseline delta; do not update goldens |
| 2 | Compress warning candidates into this gate map | `git diff --check`; no source, build, runtime, or golden changes | Revert to documentation-only if any implementation decision needs new evidence |
| 3 | `src/udicunix/src/alphabet.c` `-Wunused-variable` cleanup | Default Autotools gate, strict warning summary, user dictionaries, generated dictionaries, public headers, symbols, API/callback smoke, deterministic audio, warning budgets | Revert if user-dictionary output, dictionary output, warnings outside the target category, or behavior gates change unexpectedly |
| 4 | `src/samplosf/src/dtsamples/tunecheck.c` `-Wmissing-prototypes` cleanup for private `MakeTunerParams` | Default Autotools gate, strict warning summary, deterministic audio, symbols, headers, warning budgets | Revert if generated tuner string logic, command-line behavior, symbols, headers, dictionaries, or audio output changes unexpectedly |
| 5 | `src/samplosf/src/dtsamples/tunecheck.c` `-Wunused-variable` cleanup | Default Autotools gate, strict warning summary, deterministic audio, symbols, headers, warning budgets | Revert if callback logic, buffer processing, command-line behavior, symbols, headers, dictionaries, or audio output changes unexpectedly |
| 6 | Final readiness, PR, and merge | Final `verify_current.sh`, final `verify_cmake_subset.sh`, audio-option matrix, warning budgets, PR checks, merge policy | Stop before merge on any unexplained behavior, symbol, header, dictionary, manifest, API, callback, phoneme, timing, or audio delta |

## Selected Candidates

### Phase 3: UDICT Alphabetizer Unused Variables

Selected file and category:

- `src/udicunix/src/alphabet.c`
- strict `-Wunused-variable`

Current refreshed evidence from
`baseline-runs/next5-phase1-post-pr6/warnings-strict/warnings.tsv` shows
repeated unused-variable rows for local variables `Guard1`, `Guard2`, `i`, and
`termstrg`. The cleanup is limited to removing variables that are not read.

Rejected adjacent work:

- pointer-sign warnings in `alphabet.c`;
- text parsing changes;
- sort-order changes;
- codepage conversion changes;
- file I/O changes;
- dictionary format or output changes.

### Phase 4: Tunecheck Missing Prototype

Selected file and category:

- `src/samplosf/src/dtsamples/tunecheck.c`
- strict `-Wmissing-prototypes`

Current refreshed evidence shows `MakeTunerParams` has no previous prototype.
The planned cleanup is to make the helper file-local only if source review
confirms it is not externally referenced.

Rejected adjacent work:

- generated tuner string changes;
- command-line behavior changes;
- public API changes;
- callback or audio buffer logic changes;
- qualifier, pointer-sign, format, or timing cleanup.

### Phase 5: Tunecheck Unused Variables

Selected file and category:

- `src/samplosf/src/dtsamples/tunecheck.c`
- strict `-Wunused-variable`

Current refreshed evidence shows unused local variables in `main`,
`TTSCallbackRoutine`, and `DoFullAutoTune`. The cleanup is limited to removing
locals that are not read and not part of observable output.

Rejected adjacent work:

- callback message semantics;
- buffer processing arithmetic;
- audio file generation;
- tuner-string generation;
- command-line parsing;
- format-y2k output changes;
- const/qualifier cleanup.

## Deferred Areas

These remain deferred unless a later phase explicitly narrows and verifies
them:

- speech, parser, phoneme, LTS, VTM, HLSYN, and timing logic cleanup;
- public header or ABI changes;
- exported symbol changes;
- dictionary format or lookup behavior changes;
- live-audio routing, callback timing, queue, pipe, buffer, or backend behavior
  changes;
- `src/dapi/src/nt/opthread.c` runtime behavior changes;
- `src/dapi/src/nt/linux_audio.c` runtime behavior changes;
- default runtime wiring of `src/platform` wrappers;
- CMake promotion to the primary Linux build path;
- historical target source deletion.

## Gate Policy

Local readiness gates should keep exact comparisons. Hosted CI may use stable
symbol name/type or manifest path/type comparisons only where runner-specific
addresses, binary metadata, or optional runner-built tools differ.

If any target produces an unexplained audio, dictionary, symbol, manifest,
public header, API, callback, phoneme, text, timing, or threading delta, stop
and treat the investigation as an extra-high reasoning checkpoint.
