# CMake Overview

Phase 7 adds a side-by-side CMake build option. It does not replace Autotools,
Visual Studio project files, GitHub Actions build scripts, or packaging.

## Current scope

The initial CMake build creates:

- `tts_us`: a US English shared library with output name `libtts_us.so`.
- `say_cmake`: a minimal sample tool built from
  `src/samplosf/src/dtsamples/say.c`.
- `compile_commands.json` for analysis tooling.

Configure example:

```sh
CC=/usr/bin/gcc cmake -S . -B baseline-runs/cmake-build -DCMAKE_BUILD_TYPE=Release
cmake --build baseline-runs/cmake-build --target tts_us say_cmake -- -j1
```

`CC=/usr/bin/gcc` avoids the host `ccache` wrapper in sandboxed environments.

## Source membership

The CMake `tts_us` target intentionally starts with the main US Linux DAPI source
groups that the Autotools build uses for `libtts_us.so`:

- API sources from `src/dapi/src/api`.
- command/parser sources from `src/dapi/src/cmd`.
- LTS sources from `src/dapi/src/lts`.
- phoneme sources from `src/dapi/src/ph`.
- high-level synthesizer sources from `src/dapi/src/hlsyn`.
- kernel sources from `src/dapi/src/kernel`.
- Linux/NT compatibility sources from `src/dapi/src/nt`.
- OSF compatibility support sources used by the Unix build.
- VTM sources from `src/dapi/src/vtm`.

The CMake build does not yet build:

- non-US language variants
- dictionary compiler targets
- generated dictionary outputs
- multi-language `libtts.so`
- install/dist packaging
- GTK Speak, windict, tunecheck, or other sample tools
- Visual Studio, Windows CE, OSF, Solaris, iPAQ, or Emscripten variants

These differences are intentional and must remain documented until parity is
proven.

## Behavior status

The CMake build is compile/link scaffolding only at this stage. Runtime output
comparison remains based on the Autotools-built `dist/say` and golden WAV files.
