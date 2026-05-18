# Platform Wrapper Wiring Decision

Phase 8 decision: defer runtime wrapper wiring.
Phase 9 update: add non-runtime wrapper parity coverage, but continue to defer
runtime wiring.
Phase 10 decision: no runtime wrapper pilot is approved.
Accelerated Phase 8 update: add isolated legacy `OP_*` parity evidence, but
continue to defer runtime wiring.
Next plan Phase 10 update: add a private `OP_*` adapter scaffold for CMake-only
evidence, but keep runtime wiring deferred.
Next plan Phase 11 decision: do not add a disabled-by-default runtime wrapper
option yet.

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

## Accelerated Phase 8 OP_* Harness

Accelerated Phase 8 adds `tools/platform/opthread_smoke.c` and builds it only
as the CMake `opthread_smoke` developer target. The target links directly
against `src/dapi/src/nt/opthread.c` for evidence, but it is not installed and
is not linked into DECtalk runtime libraries.

Covered by the harness:

- `OP_CreateThread` with default stack size;
- `OP_WaitForThreadTermination` handle ownership and current Linux return
  behavior;
- `OP_GetThreadPriority` and `OP_SetThreadPriority` with the current thread
  priority;
- `OP_Sleep(0)` as a smoke-level scheduler-yield check;
- `OP_CreateMutex`, `OP_LockMutex`, `OP_UnlockMutex`, and `OP_DestroyMutex`;
- auto-reset and manual-reset `OP_CreateEvent`/`OP_WaitForEvent` semantics;
- `ThreadLock` successful lock and zero-timeout failed relock behavior.

Observed CMake verifier output:

```text
op_thread_wait_status=1
op_thread_return=77
op_thread_priority=0
op_mutex=ok
op_event_semantics=ok
op_lightweight_lock=ok
op_sleep_zero=ok
opthread_smoke=ok
```

Still not covered:

- live audio routing, device opening, callback timing, queue behavior, pipe
  behavior, buffer ownership, reset/pause/restart transitions, and backend
  state;
- exact scheduler fairness or timing guarantees;
- non-current platform `OP_*` behavior;
- runtime replacement of any `opthread.c` call path with `src/platform`.

Decision remains unchanged: no runtime wrapper pilot is approved. The new
evidence helps define adapter requirements, but `src/platform` is still not a
drop-in replacement for active DECtalk runtime code.

## Accelerated Phase 9 Adapter Decision

Decision: defer platform adapter scaffolding.

The accelerated Phase 8 `opthread_smoke` target gives useful legacy evidence,
but it also confirms that an adapter would need to preserve details that are
not represented by the current `src/platform` APIs:

- Linux `OP_WaitForThreadTermination` currently returns `1` after a successful
  join in the smoke harness while also returning the thread status value.
- Legacy thread handles are heap-allocated `pthread_t *` values owned and freed
  by the wait function.
- Priority get/set calls are part of the legacy contract even though the
  wrapper API does not expose priority.
- `ThreadLock` uses the legacy timeout-polling contract, including immediate
  failure on zero-timeout relock in the smoke harness.
- Event constants, return values, and auto-reset/manual-reset semantics must
  remain compatible with `OP_WAIT_*` behavior.

Current `dt_thread`, `dt_mutex`, `dt_event`, and `dt_time` wrappers remain
useful scaffolding, but their APIs do not encode those legacy contracts exactly.
Adding adapter files now would either duplicate `opthread.c` semantics without
a runtime user or create a misleading migration path before live runtime gates
exist.

Required before reconsidering adapter scaffolding:

- define whether adapters preserve `OP_*` names and return values exactly or
  expose a separate compatibility layer;
- add callback, queue, pipe, reset, pause, restart, and buffer-ownership
  evidence for any audio-adjacent adapter;
- keep exact public API, symbol, dictionary, manifest, and deterministic audio
  gates in the adapter plan;
- decide how non-current platform `OP_*` branches remain quarantined.

No new adapter source or header is approved by this phase.

## Next Plan Phase 10 Adapter Scaffold

Decision: add a private CMake-only adapter scaffold while continuing to defer
runtime wrapper wiring.

The adapter files `src/platform/dt_opthread_adapter.h` and
`src/platform/dt_opthread_adapter.c` wrap the current legacy `OP_*` primitives
without translating their visible contracts. The scaffold models the evidence
captured by `opthread_smoke`:

- thread handles remain legacy `HTHREAD_T` values owned by
  `OP_WaitForThreadTermination`;
- wait return values are preserved, including the current Linux smoke-observed
  successful join result;
- priority get/set calls delegate to `OP_GetThreadPriority` and
  `OP_SetThreadPriority`;
- auto-reset and manual-reset event behavior delegates to `OP_CreateEvent`,
  `OP_WaitForEvent`, `OP_SetEvent`, and `OP_ResetEvent`;
- `OP_Sleep(0)` and `ThreadLock`/`ThreadUnlock` remain delegated legacy
  behavior.

The adapter is exercised by the CMake-only `dt_opthread_adapter_smoke` target.
That smoke target links directly with `src/dapi/src/nt/opthread.c`, is not
installed, and is not linked into DECtalk runtime libraries. Its purpose is to
keep a private compatibility shape available for future migration experiments
without changing the active Linux runtime.

Observed adapter smoke output:

```text
adapter_thread_stack_size=65536
adapter_thread_wait_status=1
adapter_thread_return=91
adapter_thread_priority=0
adapter_mutex=ok
adapter_event_semantics=ok
adapter_lightweight_lock=ok
adapter_sleep_zero=ok
dt_opthread_adapter_smoke=ok
```

Still not covered:

- live audio routing, device opening, callback timing, queue behavior, pipe
  behavior, buffer ownership, reset/pause/restart transitions, and backend
  state;
- exact scheduler fairness or timing guarantees;
- non-current platform `OP_*` behavior;
- replacement of any active DECtalk runtime call path with `src/platform`.

Decision remains unchanged for runtime code: no existing DECtalk runtime source
is routed through this adapter by default.

## Next Plan Phase 11 Experimental Opt-In Decision

Decision: defer disabled-by-default runtime wrapper opt-in scaffolding.

The Phase 10 adapter gives a private compatibility shape for legacy `OP_*`
primitives, but it is still not enough evidence for a runtime build option. A
real opt-in would require at least one default runtime library, executable, or
call path to choose between `opthread.c` behavior and adapter-routed behavior.
That would create an unproven second threading path around API initialization,
audio-adjacent thread ownership, queues, pipes, callbacks, and reset/pause/
restart transitions.

No CMake, Autotools, installed-header, public-API, or runtime-library option is
added in this phase. Adding an option such as
`DECTALK_EXPERIMENTAL_OPTHREAD_ADAPTER` would be premature until the option can
be compiled and tested against runtime-specific evidence rather than only
primitive smoke tests.

Blocking gaps before any future opt-in:

- deterministic evidence for API-owned thread lifecycle and shutdown behavior;
- callback ordering and callback repeatability evidence when runtime threads
  are active;
- queue, pipe, and buffer-ownership evidence around `src/dapi/src/nt` runtime
  code;
- reset, pause, restart, and audio-timing evidence, ideally without requiring
  live audio hardware first;
- default and opt-in comparisons for exported symbols, public headers,
  dictionaries, user dictionaries, manifests, warning budgets, and deterministic
  WAV output;
- a rollback plan that removes only the opt-in path without disturbing the
  authoritative default build.

Future opt-in naming should be explicit and experimental, disabled by default,
excluded from installed public APIs, and documented as non-production until the
runtime gates above pass. Until then, the only approved wrapper work remains
CMake-only smoke or compile evidence that does not alter default runtime
routing.
