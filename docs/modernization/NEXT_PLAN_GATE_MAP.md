# Next Plan Gate Map

This note maps the next modernization plan to concrete work targets and
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

## Target Map

| Phase | Target | Risk | Required gates |
| --- | --- | --- | --- |
| 3 | Deterministic phoneme or text-mode baseline feasibility | medium until output source is proven deterministic; extra-high before accepting any baseline | inventory only unless feasible; if accepted, exact capture/compare scripts plus `verify_current.sh`, public header audit, symbol comparison, dictionary comparison, API smoke, and golden audio comparison |
| 4 | Public API smoke matrix expansion in `tools/baseline/` | medium because it exercises public API behavior, but test-only if public headers and runtime code are unchanged | installed-header compile, API smoke output, public header audit, exact exported symbols, dictionaries, one-shot and expanded US audio |
| 5 | CMake staged packaging gap closure | medium to high because install layout and source membership can drift | `verify_cmake_subset.sh` with exact local `libtts.so` symbols, Autotools `verify_current.sh`, detailed manifest capture/compare, dictionary comparison, one-shot and expanded US audio, language-library symbol name/type sets |
| 6 | CMake parity decision checkpoint | low if documentation-only | manifest and symbol evidence from Phase 5, no source changes unless separately approved |
| 7 | Low-risk warning budget expansion in auxiliary files | low only when limited to private tools/samples and simple warning categories | targeted warning inventory, `verify_current.sh`, warning budget check, public header and symbol comparisons when relevant |
| 8 | API-boundary warning cleanup | high | expanded API smoke matrix, public header audit, exact exported symbols, dictionaries, one-shot and expanded US audio, warning summary and budget check |
| 9 | Platform wrapper parity harness | high if it adds runtime-adjacent tests; must remain no-runtime-wiring | CMake smoke/parity test, `verify_cmake_subset.sh` when CMake targets change, Autotools gate if shared scripts or source membership change |
| 10 | Runtime wrapper pilot decision | extra-high | decision-only unless explicitly approved; any implementation requires `verify_current.sh`, API smoke, CMake subset, symbols, public headers, dictionaries, audio, warning budget, and a candidate-specific OP/platform parity test |
| 11 | Macro and historical target quarantine audit | low for documentation; high for macro/build changes | documentation-only by default; build-system quarantine changes require Autotools and CMake gates plus macro classification evidence |
| 12 | Final readiness review | low if documentation-only | final Autotools and CMake accepted gates plus explicit limitations |

## Preferred Initial Targets

The safest implementation targets after Phase 2 are:

- CMake staged-layout work where the Autotools source path is explicit and the
  staged artifact can be compared without changing runtime code.
- Public API smoke matrix expansion in `tools/baseline/`, using installed
  headers and no-audio deterministic calls.
- Low-risk warning budget expansion in private auxiliary code, one warning
  class at a time.
- Platform-wrapper parity tests that compare semantics without routing runtime
  code through wrappers.

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
