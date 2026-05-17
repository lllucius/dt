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

The CMake-only `dt_platform_smoke` target exercises these wrappers as a
developer check without installing them or routing DECtalk runtime behavior
through them.
