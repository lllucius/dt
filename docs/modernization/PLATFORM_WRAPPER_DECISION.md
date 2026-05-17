# Platform Wrapper Wiring Decision

Phase 8 decision: defer runtime wrapper wiring.

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

`src/dapi/src/nt/linux_audio.c` owns live audio backend selection, device
opening, player-thread lifecycle, message queues, callback notification, buffer
state, timing, reset/pause/restart transitions, and backend-specific routing.
`src/platform/dt_audio_backend.*` intentionally exposes compile-time backend
metadata only. It does not open devices, route buffers, preserve callback
timing, or model the `WINE_WAVEOUT` state machine.

## Smallest Candidate

No runtime wiring candidate is approved for Phase 9.

The smallest plausible future candidate is not a runtime redirect. It is a
parity-test step for `OP_*` and `dt_*` behavior:

- add a dedicated OP/thread/event parity smoke test that exercises event
  manual-reset and auto-reset behavior, finite timeout behavior, thread
  create/join behavior, mutex lock/unlock behavior, and sleep/yield behavior;
- compare that evidence with the existing `dt_platform_smoke` behavior;
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

Defer Phase 9 runtime wrapper wiring. The next useful step is to add parity
tests around the legacy `OP_*` behavior and decide whether `src/platform` should
grow adapter APIs that preserve those contracts exactly.

This keeps the current Linux runtime behavior under the established Autotools
path while preserving `src/platform` as isolated scaffolding for future
modernization.
