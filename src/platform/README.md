# DECtalk Platform Scaffolding

This directory is side-by-side scaffolding for future platform abstraction work.
It is not wired into the current Autotools, Visual Studio, or runtime code paths.

The first rule for this directory is to wrap existing behavior before replacing
it. Threading, audio, callback, queue, timing, and dictionary behavior must not
be redirected here until baseline checks cover the affected behavior.
