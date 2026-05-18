# Active Plan Gate Map

This note maps the accelerated modernization plan to concrete work targets and
verification gates. It is planning documentation only; it does not approve
behavior changes.

## Scope Rules

- Current implementation target: Linux.
- Autotools remains authoritative.
- CMake remains side-by-side until parity is proven and explicitly accepted.
- Speech output, phoneme output, dictionary behavior, public API signatures,
  exported symbol names, install layout, live-audio behavior, threading model,
  sample rate, and default voice are unchanged unless a later phase explicitly
  approves and verifies a change.
- Historical targets remain preserved and are not deleted.
- Extra-high reasoning is active for the risky block, so the plan can proceed
  without additional reasoning-change prompts unless a regression or ambiguity
  appears.

## Target Map

| Phase | Target | Risk | Required gates |
| --- | --- | --- | --- |
| 1 | Post-merge baseline refresh | low if documentation-only after gates pass | `verify_current.sh`, `verify_cmake_subset.sh`, warning counts, symbols, manifests, dictionaries, public headers, API smoke, one-shot and expanded US audio |
| 2 | Knowledge base reconciliation | low if documentation-only | `git diff --check`; no source, build, baseline, or runtime changes |
| 3 | Warning inventory refresh and candidate selection | low if inventory-only | Phase 1 warning summaries, warning-risk classification, explicit Phase 4 candidate and rejected candidates |
| 4 | Low-risk auxiliary warning cleanup | low only when limited to private tools/samples and simple warning categories | targeted warning evidence, `verify_current.sh`, warning budget check, public headers, symbols, dictionaries, API smoke, one-shot and expanded US audio |
| 5 | Deterministic coverage gap map | low if documentation-only | existing API, audio, phoneme feasibility, CMake, and platform-smoke evidence; no new baselines accepted |
| 6 | CMake detailed parity policy | low if documentation/comparison-only | current Autotools and CMake detailed manifests, language-library symbol name/type evidence, audio-option blocker classification |
| 7 | Public API smoke matrix expansion | medium to high because it exercises more public API behavior, but test-only if public headers and runtime code are unchanged | installed-header compile, API smoke output, public header audit, exact exported symbols, dictionaries, one-shot and expanded US audio |
| 8 | Legacy `OP_*` parity harness | high if it adds runtime-adjacent tests; must remain no-runtime-wiring | deterministic OP/platform parity test, CMake and/or Autotools harness integration, `verify_cmake_subset.sh` when CMake targets change, Autotools gate when shared scripts or source membership change |
| 9 | Platform wrapper adapter decision | extra-high | decision-only unless isolated non-runtime scaffolding is justified; any new source/header needs standard docs and CMake smoke coverage |
| 10 | CMake live-audio option parity scaffolding | extra-high | `verify_cmake_subset.sh`, Autotools `verify_current.sh`, compile-command/config evidence, symbols, manifests, dictionaries, public headers, API smoke, one-shot and expanded US audio |
| 11 | API-boundary warning pilot | extra-high | expanded API smoke, public header audit, exact exported symbols, dictionaries, one-shot and expanded US audio, warning summary and budget check |
| 12 | CMake promotion readiness decision | extra-high; documentation-only unless separately approved | manifest and symbol evidence from Phase 10, compile-command evidence, explicit promotion blockers or future promotion plan |
| 13 | Final readiness review, PR, and merge | extra-high | final Autotools and CMake accepted gates, warning budgets, explicit limitations, PR checks, merge accepted by repository policy |

## Preferred Throughput Strategy

The fastest useful route is not broad refactoring. It is to keep every risky
implementation behind deterministic gates and to defer work when evidence is
missing instead of widening scope mid-phase.

Safe acceleration choices:

- keep extra-high reasoning active through the full plan;
- avoid reasoning-change prompts because all remaining risky phases are grouped
  together;
- keep warning cleanup to one file and one category at a time;
- prefer non-runtime tests and scaffolding before runtime wrapper work;
- keep CMake side-by-side and defer promotion to a separate future plan.

## Deferred Areas

These remain deferred until a later phase explicitly approves them:

- speech, phoneme, parser, LTS, VTM, HLSYN, and timing logic cleanup
- public header or ABI changes
- exported symbol changes
- dictionary format or lookup behavior changes
- live-audio routing, callback timing, queue, pipe, or backend behavior changes
- `src/dapi/src/nt/opthread.c` runtime behavior changes
- `src/dapi/src/nt/linux_audio.c` runtime behavior changes
- runtime wiring of `src/platform` wrappers
- CMake promotion to the primary Linux build path
- historical target source deletion

## Gate Policy

Local readiness gates should keep exact comparisons. Hosted CI may use stable
symbol name/type or manifest path/type comparisons only where runner-specific
addresses, binary metadata, or optional runner-built tools differ.

If any target produces an unexplained audio, dictionary, symbol, manifest,
public header, API, phoneme, or timing delta, stop and treat the investigation
as an extra-high reasoning checkpoint.
