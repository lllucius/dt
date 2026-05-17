# CMake Overview

The side-by-side CMake build is optional Linux build scaffolding. It does not
replace Autotools, Visual Studio project files, GitHub Actions build scripts, or
packaging, and Autotools remains authoritative until full parity is proven.

Non-Linux CMake experimentation requires
`-DDECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON`. When that option is enabled, CMake
also defines `DECTALK_ENABLE_LEGACY_TARGET_SOURCE` for the platform scaffolding
so historical target macros are explicit rather than accidental.

## Current scope

The Phase 8 CMake build creates:

- `tts_us`, `tts_uk`, `tts_sp`, `tts_gr`, `tts_la`, and `tts_fr`, with output
  names `libtts_<lang>.so`.
- `tts_multi`, with output name `libtts.so`.
- `dic_us`, `dic_uk`, `dic_sp`, `dic_gr`, `dic_la`, and `dic_fr`.
- generated dictionaries under the build tree, installed into the staged
  `dic/` directory.
- `say_cmake`, with output name `say`, linked through `libtts.so` like the
  Autotools sample.
- `dectalk_cmake_stage`, which installs the CMake-built subset into
  `baseline-runs/.../cmake-dist` by default.
- `compile_commands.json` for analysis tooling.

Configure and stage example:

```sh
CC=/usr/bin/gcc cmake -S . -B baseline-runs/cmake-build -DCMAKE_BUILD_TYPE=Release
cmake --build baseline-runs/cmake-build --target dectalk_cmake_stage -- -j1
```

`CC=/usr/bin/gcc` avoids the host `ccache` wrapper in sandboxed environments.

Subset verification:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/current-cmake \
  --expected tests/golden
```

The subset verifier configures and builds the CMake stage, checks
`compile_commands.json`, compares generated dictionaries, compares the original
US English WAVs and expanded US audio suites against golden outputs, compares
`libtts.so` symbols exactly, and compares language-library exported symbol
name/type sets.

## Source membership

The CMake language libraries use the main Linux DAPI source groups that the
Autotools build uses for `libtts_<lang>.so`:

- API sources from `src/dapi/src/api`.
- command/parser sources from `src/dapi/src/cmd`.
- LTS sources from `src/dapi/src/lts`.
- phoneme sources from `src/dapi/src/ph`.
- high-level synthesizer sources from `src/dapi/src/hlsyn`.
- kernel sources from `src/dapi/src/kernel`.
- Linux/NT compatibility sources from `src/dapi/src/nt`.
- OSF compatibility support sources used by the Unix build.
- VTM sources from `src/dapi/src/vtm`.

The CMake `libtts.so` target uses the Autotools multi-language sources from
`src/dtalkml/src/dtalk_ml.c` and `src/dtalkml/src/init.c`. The staged `say`
binary links to this `libtts.so` target, so `-l us` and the multi-language
loader path are available during CMake runtime checks.

## Audio options

The CMake build does not currently expose live audio backend options. It does
not pass `USE_ALSA`, `USE_PULSEAUDIO`, or `DISABLE_AUDIO` into DECtalk runtime
targets, and it does not replace Autotools audio probing. See
`docs/modernization/AUDIO_BACKEND.md` for the current backend inventory and the
Phase 11 containment scaffolding.

The dictionary custom commands intentionally pass build-relative output paths to
`dic_<lang>`. The dictionary compiler treats leading `/` arguments as options,
so absolute output paths are not accepted by its current command-line parser.

## Remaining Gaps

The CMake staged tree is still a build-verification subset. It does not yet
stage the full Autotools dist layout, including the documentation tree, bitmap
assets, source sample tree, `usr/bin` symlinks, and additional sample or helper
tools such as `aclock`, `dtmemory`, `gspeak`, `windic`, and user-dictionary
tools.

Do not declare CMake packaging parity until this staged-layout gap is closed and
the manifest comparison is exact or every remaining difference is explicitly
approved.

## Phase 18 Promotion Recommendation

Recommendation: do not promote CMake to the primary Linux build path yet.
Autotools should remain authoritative.

Evidence supporting continued side-by-side status:

- CMake configures successfully and emits `compile_commands.json`.
- `dectalk_cmake_stage` builds the current CMake subset.
- generated CMake dictionaries match `tests/golden/dictionaries` exactly.
- CMake-staged `say` produces exact US English golden WAVs for speakers 0
  through 8.
- CMake-staged `libtts.so` matches the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets match the committed
  baselines, while full address-sorted symbol files still differ from
  Autotools output.
- Ubuntu CI now runs `tools/baseline/verify_cmake_subset.sh`.
- detailed manifest evidence from Phase 15 shows CMake staging is still a
  subset: 90 detailed CMake staged lines versus 1,126 detailed Autotools lines,
  with missing documentation, bitmap assets, sample source trees, `/usr/bin`
  symlinks, `README`, and additional helper/sample/user-dictionary tools.

CMake should become a primary Linux build path only after the staged layout gap
is closed or each remaining packaging difference is explicitly accepted, and
after the symbol-address/order differences for language libraries are either
eliminated or explicitly accepted as non-ABI-relevant.

## Phase 8 Verification

Phase 8 checks used `baseline-runs/phase8-cmake-expanded-2/cmake-dist`:

- CMake configured successfully and emitted `compile_commands.json`.
- `dectalk_cmake_stage` built and installed the CMake subset.
- generated CMake dictionaries matched `tests/golden/dictionaries` exactly.
- CMake-staged `say` generated US English WAVs for speakers 0 through 8 that
  matched `tests/golden/audio/us` exactly.
- `libtts.so` matched the committed symbol capture exactly.
- language-library symbol addresses and address-sorted order differ from the
  Autotools capture, but sorted symbol-name sets match for all `libtts_<lang>.so`
  libraries.
- staged manifest comparison is intentionally different at this phase:
  541 Autotools dist entries are not yet staged by CMake, and CMake has no extra
  entries.
- Autotools behavior verification still passed with
  `tools/baseline/verify_current.sh --run-dir baseline-runs/phase8-behavior --expected tests/golden`.
