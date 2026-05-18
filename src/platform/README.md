# DECtalk Platform Scaffolding

This directory is side-by-side scaffolding for future platform abstraction work.
It is not wired into the current Autotools, Visual Studio, or runtime code paths.

The first rule for this directory is to wrap existing behavior before replacing
it. Threading, audio, callback, queue, timing, and dictionary behavior must not
be redirected here until baseline checks cover the affected behavior.

Current wrappers:

- `dt_time.*`: POSIX monotonic time and millisecond sleep helpers.
- `dt_filesystem.*`: POSIX path classification, path joining, and existence
  checks.
- `dt_thread.*`, `dt_mutex.*`, and `dt_event.*`: POSIX thread and
  synchronization wrappers compiled only as scaffolding.
- `dt_audio_backend.*`: compile-time audio backend metadata used to document
  current backend availability without opening devices or changing runtime
  audio routing.
- `dt_legacy_targets.*`: compile-time historical target inventory and opt-in
  gate for platform scaffolding.
- `dt_opthread_adapter.*`: private compatibility adapter that delegates to the
  legacy `OP_*` thread, mutex, event, sleep, priority, and lightweight-lock
  primitives for CMake-only migration evidence.

The CMake-only `dt_platform_smoke` target exercises these wrappers as a
developer check without installing them or routing DECtalk runtime behavior
through them. It covers simple thread join, mutex lock/unlock, auto-reset and
manual-reset event waits, finite event timeouts, path helpers, compile-time
audio metadata, and historical target inventory. It does not prove parity for
legacy `OP_*` stack size, priority, handle ownership, scheduler-yield,
lightweight-lock, live-audio, callback, queue, or buffer behavior.

The CMake-only `opthread_smoke` target exercises selected legacy `OP_*`
semantics directly against `src/dapi/src/nt/opthread.c`. It records current
thread create/join, priority, event, mutex, sleep, and lightweight-lock
behavior as comparison evidence for future adapter work. It is not installed,
does not link to DECtalk runtime libraries, and does not route any runtime
threading or audio behavior through `src/platform`.

The CMake-only `dt_opthread_adapter_smoke` target exercises the private
`dt_opthread_adapter.*` layer against the same legacy `opthread.c`
implementation. The adapter intentionally preserves the observed legacy return
values and ownership model instead of translating them into the cleaner
`dt_thread.*`, `dt_mutex.*`, `dt_event.*`, or `dt_time.*` APIs. It remains a
developer-only scaffold and must not be installed or linked into DECtalk
runtime libraries until a later phase explicitly approves runtime wiring.
