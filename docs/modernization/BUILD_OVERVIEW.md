# Build Overview

Phase 1 records the build paths that exist before modernization. This is an
inventory, not a support promise.

## Current intended target

- Linux is the current intended supported target for modernization work.

## Build systems present

- Autotools/Make under `src/`.
  - `src/configure.ac` generates `src/configure`.
  - `src/Makefile.in` is the top-level Unix build template.
  - `src/Makefile.sub.in` dispatches per-language builds.
  - Module Makefile templates exist under `src/dapi/src`, `src/dtalkml/src`,
    `src/samplosf/src`, `src/licunix/src`, and `src/udicunix/src`.
- GitHub Actions in `.github/workflows/build.yml`.
  - Ubuntu and macOS jobs use Autotools/Make.
  - Visual Studio 6 and Visual Studio 2022 jobs are still present.
- Windows project files.
  - `src/DECtalk.sln` and many `.vcxproj`, `.dsp`, and `.dsw` files are present.
  - `devops/vs6`, `devops/vs2022`, and `devops/vs2022-ARM64` contain batch
    build/copy entry points.
- Docker build path.
  - `Dockerfile` uses Debian bullseye and runs `autoreconf -i`, `./configure`,
    and `make`.
  - `docker-compose.yml` mounts the repository and runs that Dockerfile.
- Side-by-side CMake build.
  - `CMakeLists.txt` currently builds an initial US `libtts_us.so`, `say_cmake`,
    and `compile_commands.json`.
  - The CMake path is Linux-only by default; non-current targets require
    `DECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON`.
  - Scope and source-membership differences are documented in
    `docs/modernization/CMAKE_OVERVIEW.md`.
- Emscripten/WebAssembly port under `ports/emscripten`.

## Phase 1 local baseline command sequence

The local Phase 1 baseline used the current host as-is:

```sh
cd src
autoreconf -i
CC=/usr/bin/gcc ./configure
make
make clean
make WARN_FLAGS='-Wall -Wextra -Wpedantic -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-declarations -Wpointer-arith -Wcast-qual -Wwrite-strings -Wbad-function-cast -Wnested-externs'
```

`CC=/usr/bin/gcc` avoids the host `ccache` wrapper, which failed in the sandbox.

## Phase 1 observed host

- Kernel: Linux x86_64 under WSL2.
- Compiler: GCC 15.2.1.
- Autoconf: 2.72.
- Automake: 1.18.1.
- Make: GNU Make 4.4.1.
- Optional packages not found by `pkg-config`: `gtk+-2.0`, `alsa`, `libpulse`.

## Phase 1 build result

- `autoreconf -i`: succeeded.
- `./configure`: succeeded with `CC=/usr/bin/gcc`.
- Default single-threaded `make`: succeeded.
- Strict warning single-threaded rebuild: succeeded.
- Generated files and full logs are ignored under `baseline-runs/phase1/`.
