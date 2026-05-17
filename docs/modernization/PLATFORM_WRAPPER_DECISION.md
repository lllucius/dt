# Platform Wrapper Wiring Decision

Phase 8 decision: defer runtime wrapper wiring.
Phase 9 update: add non-runtime wrapper parity coverage, but continue to defer
runtime wiring.
Phase 10 decision: no runtime wrapper pilot is approved.

No `src/platform` wrapper is approved for runtime integration yet. The existing
wrappers remain useful CMake-only scaffolding, but they are not behaviorally
proven replacements for the legacy runtime paths in `opthread.c`,
`linux_audio.c`, or API-adjacent thread/audio ownership.

## Runtime Ownership Compared

`src/dapi/src/nt/opthread.c` owns the active thread, mutex, event, sleep, and
lightweight lock behavior used by the current Linux runtime. Important behavior
that is not yet covered by wrapper parity tests includes:

- `OP_CreateThread` accepts a stack-size parameter and returns a heap-allocated
  `pthread_t *` handle.
- `OP_WaitForThreadTermination` has legacy platform-specific signatures,
  including timeout-bearing POSIX paths.
- `OP_GetThreadPriority` and `OP_SetThreadPriority` expose priority behavior
  that `dt_thread.*` does not wrap.
- `OP_CreateEvent` allocates an event plus separately allocated mutex and
  condition handles, and `OP_WaitForEvent` uses legacy return constants and
  auto-reset behavior.
- `OP_Sleep(0)` yields the scheduler, while non-zero sleeps use the existing
  `nanosleep` implementation and its current edge cases.
- `ThreadLock` and `ThreadUnlock` implement separate lightweight lock semantics
  with timeout polling.

`src/platform/dt_thread.*`, `dt_mutex.*`, `dt_event.*`, and `dt_time.*` are
cleaner POSIX wrappers, but their APIs are not drop-in equivalents for those
legacy contracts.

## Phase 9 Parity Harness

Phase 9 expands the CMake-only `dt_platform_smoke` harness and runs it from
`tools/baseline/verify_cmake_subset.sh`. The harness still does not link into
Autotools runtime libraries or route DECtalk runtime behavior through
`src/platform`.

Covered by the harness:

- `dt_thread_create`, `dt_thread_join`, and `dt_thread_destroy` can run a
  simple worker and return a successful thread status.
- `dt_mutex_lock` and `dt_mutex_unlock` protect shared smoke-test state.
- `dt_event_wait` covers auto-reset consumption after one waiter, finite
  timeout after consumption, manual-reset repeated waits while signaled, and
  timeout after manual reset.
- `dt_sleep_milliseconds(0)` and `dt_monotonic_milliseconds` remain simple
  smoke checks only.
- filesystem path join/existence, compile-time audio backend metadata, and
  historical-target inventory still run as scaffolding checks.

Still not covered:

- legacy `OP_CreateThread` stack-size handling;
- `OP_WaitForThreadTermination` timeout and handle ownership semantics;
- `OP_GetThreadPriority` and `OP_SetThreadPriority`;
- `OP_Sleep(0)` scheduler-yield equivalence;
- `ThreadLock` and `ThreadUnlock` timeout-polling behavior;
- live audio routing, device opening, callback timing, queue behavior, buffer
  ownership, reset/pause/restart transitions, and backend-specific state.

The added harness improves wrapper evidence but does not prove drop-in parity
with `opthread.c` or `linux_audio.c`.

`src/dapi/src/nt/linux_audio.c` owns live audio backend selection, device
opening, player-thread lifecycle, message queues, callback notification, buffer
state, timing, reset/pause/restart transitions, and backend-specific routing.
`src/platform/dt_audio_backend.*` intentionally exposes compile-time backend
metadata only. It does not open devices, route buffers, preserve callback
timing, or model the `WINE_WAVEOUT` state machine.

## Smallest Candidate

No runtime wiring candidate is approved after Phase 10.

The smallest plausible future candidate remains another parity-test step, not a
runtime redirect:

- add a dedicated legacy `OP_*` smoke test that exercises stack-size handling,
  timeout behavior, heap-allocated handle ownership, priority calls,
  scheduler-yield behavior, and lightweight lock timeout polling;
- compare that evidence with the expanded `dt_platform_smoke` behavior;
- only then consider a small adapter, if the wrapper API is adjusted to preserve
  the legacy `OP_*` contract.

## Required Gates Before Runtime Wiring

Before any future wrapper pilot changes runtime code, run at least:

- `tools/baseline/verify_current.sh --run-dir <run> --expected tests/golden`
- `tools/baseline/check_api_smoke.sh --out <run>/api-smoke`
- `tools/baseline/verify_cmake_subset.sh --run-dir <run>-cmake --expected tests/golden`
- exported symbol comparison
- public header audit
- dictionary and user-dictionary comparison
- original and expanded US golden audio comparison
- warning budget check
- a new OP/platform parity smoke test covering the exact wrapper candidate

For any audio backend routing change, require additional callback, queue,
sample-count, and timing evidence. Live audio hardware or a controlled virtual
audio backend should be part of that evidence before replacing
`linux_audio.c` behavior.

## Recommendation

Continue to defer runtime wrapper wiring. The next useful step is to add parity
tests around the legacy `OP_*` behavior and decide whether `src/platform` should
grow adapter APIs that preserve those contracts exactly.

This keeps the current Linux runtime behavior under the established Autotools
path while preserving `src/platform` as isolated scaffolding for future
modernization.

## Phase 10 Pilot Decision

Decision: defer all runtime wrapper pilots.

The Phase 9 evidence is useful but not strong enough to route any existing
runtime path through `src/platform`. The expanded `dt_platform_smoke` harness
covers standalone wrapper behavior, not drop-in parity for legacy runtime
contracts. A safe pilot still needs legacy `OP_*` evidence for stack size,
priority, timeout handling, handle ownership, scheduler yield, and lightweight
lock behavior. Audio routing remains higher risk because no live-audio,
callback-timing, queue, pipe, or buffer-ownership parity harness exists.

No exact file, wrapper, behavior gate, or rollback plan is proposed for
implementation in this phase because the prerequisite evidence is incomplete.
