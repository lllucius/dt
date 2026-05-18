# DECtalk Modernization Notes

This directory is the modern maintenance entry point for DECtalk. The current
modernization target is Linux. Historical build paths and platform branches are
preserved, but they are not the current cleanup target unless explicitly called
out by a phase plan.

## Start Here

- `BUILD_OVERVIEW.md`: current build systems, supported modernization target,
  and build-path inventory.
- `BASELINE_PROCEDURE.md`: behavior baseline workflow for builds, symbols,
  dictionaries, user dictionaries, audio, and manifests.
- `RISK_AREAS.md`: files and behaviors that should not be changed casually.
- `WARNING_INVENTORY.md`: warning categories, ownership, and cleanup order.
- `MACRO_INVENTORY.md`: platform, language, product, and historical macro
  classification.

## Verification

- `tools/baseline/verify_current.sh --run-dir baseline-runs/current --expected tests/golden`
  runs the current Linux verification gate.
- `PUBLIC_API_AUDIT.md` documents installed headers, header isolation checks,
  and exported-symbol policy.
- `PACKAGING_LAYOUT.md` documents the Autotools `dist/` layout and detailed
  manifest modes.
- `DICTIONARY_BASELINE.md` documents dictionary and user-dictionary checks.
- `READINESS_REVIEW.md` records the final readiness state, verification
  evidence, limitations, and recommended higher-risk follow-on objectives for
  the current modernization plan.
- `BASELINE_EXPANSION_PLAN.md` recommends the next deterministic baselines to
  add before higher-risk warning and API cleanup.
- `NEXT_PLAN_GATE_MAP.md` maps the next plan's higher-risk targets to required
  behavior gates before source changes begin.
- `PHONEME_BASELINE_FEASIBILITY.md` records why no new phoneme or text-mode
  golden baseline was accepted before public API smoke coverage is expanded.

## Build System Status

- `CMAKE_OVERVIEW.md`: side-by-side CMake scope, parity status, and promotion
  limitations.
- `HISTORICAL_TARGETS.md`: preserved non-current targets and explicit opt-in
  rules.
- `AUDIO_BACKEND.md`: current live audio backend behavior and containment
  scaffolding.
- `PLATFORM_WRAPPER_DECISION.md`: Phase 8 decision on whether `src/platform`
  wrappers are ready for runtime wiring.

## Cleanup Boundaries

- `API_BOUNDARY_WARNINGS.md`: API-adjacent warning cleanup notes.

Do not claim behavior preservation from documentation alone. Use the baseline
scripts and record the exact checks that were run.
