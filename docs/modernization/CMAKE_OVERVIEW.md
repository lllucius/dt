# CMake Overview

The side-by-side CMake build is optional Linux build scaffolding. It does not
replace Autotools, Visual Studio project files, GitHub Actions build scripts, or
packaging, and Autotools remains authoritative until full parity is proven.

Non-Linux CMake experimentation requires
`-DDECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON`. When that option is enabled, CMake
also defines `DECTALK_ENABLE_LEGACY_TARGET_SOURCE` for the platform scaffolding
so historical target macros are explicit rather than accidental.

## Current scope

The current CMake build creates:

- `tts_us`, `tts_uk`, `tts_sp`, `tts_gr`, `tts_la`, and `tts_fr`, with output
  names `libtts_<lang>.so`.
- `tts_multi`, with output name `libtts.so`.
- `dic_us`, `dic_uk`, `dic_sp`, `dic_gr`, `dic_la`, and `dic_fr`.
- generated dictionaries under the build tree, installed into the staged
  `dic/` directory.
- `say_cmake`, with output name `say`, linked through `libtts.so` like the
  Autotools sample.
- `aclock_cmake` and `dtmemory_cmake`, with output names `aclock` and
  `dtmemory`.
- language-specific sample binaries `say_demo_us`, `say_demo_uk`,
  `say_demo_sp`, `say_demo_gr`, `say_demo_la`, and `say_demo_fr`.
- language-specific `tunecheck_*` binaries and `udic_*` user-dictionary tools.
- helper binaries `dump_vdf` and `mfg_load`.
- generated sample text files installed under `src/DECtalk/dtsamples/`.
- staged `/usr/bin` symlinks matching the Autotools path/type layout.
- `dectalk_cmake_stage`, which installs the CMake-built subset into
  `baseline-runs/.../cmake-dist` by default.
- `compile_commands.json` for analysis tooling.
- developer smoke targets `dt_platform_smoke` and `opthread_smoke`, which are
  built by the CMake subset verifier but are not installed.

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

The CMake build exposes side-by-side cache options for the Autotools audio macro
surface:

- `DECTALK_CMAKE_DISABLE_AUDIO`, which defines `DISABLE_AUDIO` when enabled.
- `DECTALK_CMAKE_USE_ALSA`, which defines `USE_ALSA` when enabled and audio is
  not disabled.
- `DECTALK_CMAKE_USE_PULSEAUDIO`, which defines `USE_PULSEAUDIO` when enabled
  and audio is not disabled.

The default values are all `OFF`, preserving the current verified CMake state:
OSS metadata is visible on Linux, ALSA and PulseAudio are off, and audio is not
disabled. These options do not replace Autotools probing and do not prove
live-audio behavior. See `docs/modernization/AUDIO_BACKEND.md` for the current
backend inventory and containment scaffolding.

The dictionary custom commands intentionally pass build-relative output paths to
`dic_<lang>`. The dictionary compiler treats leading `/` arguments as options,
so absolute output paths are not accepted by its current command-line parser.

## Remaining Gaps

The CMake staged tree has exact basic path/type parity with the Autotools staged
tree after the Phase 5 packaging update. The basic manifest comparison now has
589 Autotools entries, 589 CMake entries, no missing paths, and no extra paths.

Do not declare full CMake packaging parity yet. The detailed metadata-hash
manifest still differs, although both detailed manifests have 1,126 lines. The
remaining detailed differences are metadata, size, and hash differences for
CMake-built binaries, plus the `doc/DECtalk/html` directory size. These are not
path/type omissions, but they still need an explicit promotion decision before
CMake can become authoritative.

CMake now exposes side-by-side live-audio macro options for `DISABLE_AUDIO`,
`USE_ALSA`, and `USE_PULSEAUDIO`, but those options do not replace Autotools
probing, link ALSA or PulseAudio libraries, or certify live-audio hardware
behavior. Autotools remains authoritative.

## Phase 6 Parity Decision

Recommendation: continue side-by-side and defer CMake promotion. CMake is now a
stronger verification build, but it is not ready to become the authoritative
Linux build path.

Current parity evidence:

- Basic staged path/type parity is exact: 589 Autotools entries, 589 CMake
  entries, no missing CMake paths, and no CMake-only paths.
- The updated CMake subset verifier passes: generated dictionaries match, US
  one-shot audio matches for speakers 0 through 8, expanded US audio suites
  match for speakers 0 through 8, `libtts.so` symbols match, language-library
  symbol name/type sets match, and `compile_commands.json` is present.
- The authoritative Autotools verifier still passes after the CMake packaging
  changes.

Remaining differences and risk:

- Detailed metadata-hash manifest: medium risk for promotion. Both detailed
  manifests have 1,126 lines, but CMake-built binaries have different sizes and
  hashes, and `doc/DECtalk/html` directory metadata differs. This is not a
  path/type gap, but release packaging has not reached byte-level artifact
  equivalence.
- Language-library full symbol captures: low to medium risk for promotion.
  Exported symbol name/type sets match for all language libraries, but the full
  address/order symbol captures differ for `libtts_us.so`, `libtts_uk.so`,
  `libtts_sp.so`, `libtts_la.so`, `libtts_gr.so`, and `libtts_fr.so`.
  `libtts.so` matches the committed symbol capture exactly.
- Live audio backend options: medium risk for promotion. CMake still does not
  expose the Autotools `USE_ALSA`, `USE_PULSEAUDIO`, or `DISABLE_AUDIO`
  configuration surface.

Future promotion work should either eliminate these differences or define and
approve an explicit acceptance policy for them. Until then, Autotools remains
the authoritative Linux build and CMake should remain side-by-side.

## Accelerated Plan Phase 6 Detailed Parity Policy

Phase 6 rechecked detailed CMake packaging evidence from the accelerated
post-merge baseline:

```sh
tools/baseline/compare_manifest.sh \
  --expected baseline-runs/next3-phase1-post-merge/dist-manifest-detailed.txt \
  --actual baseline-runs/next3-phase1-post-merge-cmake/dist-manifest-detailed.txt \
  --out baseline-runs/next3-phase6-cmake-detailed-vs-autotools.diff
```

Result: `manifest: different`.

Accepted evidence:

- basic staged path/type parity remains achieved.
- both detailed manifests contain 1,126 lines.
- CMake-generated dictionaries match accepted dictionary baselines.
- CMake deterministic US audio checks match accepted one-shot and expanded WAV
  baselines.
- CMake-staged `libtts.so` matches the committed exact exported-symbol
  baseline.
- CMake language-library exported symbol name/type sets match committed
  baselines.

Unaccepted differences:

- detailed metadata, size, and SHA-256 hashes still differ for CMake-built
  binaries.
- `doc/DECtalk/html` directory metadata still differs.
- CMake models the Autotools live-audio macro surface for `DISABLE_AUDIO`,
  `USE_ALSA`, and `USE_PULSEAUDIO`, but non-default live-audio behavior is not
  certified and CMake does not replace Autotools probing or backend linkage.

Promotion policy:

- exact basic path/type parity is necessary but not sufficient for promotion.
- detailed metadata/hash differences may be accepted only by a future explicit
  CMake promotion plan after reviewing each difference class.
- binary size/hash differences are not automatically behavior regressions while
  deterministic dictionaries, symbols, API smoke, and audio checks pass, but
  they are release-packaging differences and remain promotion blockers.
- live-audio option parity is modeled only as side-by-side macro state; live
  backend probing, linkage, device selection, callbacks, queues, and timing
  must still be certified or explicitly waived before CMake can become
  authoritative for Linux.
- this phase does not promote CMake and does not remove Autotools.

## Accelerated Plan Phase 12 Readiness Decision

Recommendation: continue side-by-side and defer CMake promotion.

Current CMake evidence:

- `tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next3-phase12-cmake-readiness --expected tests/golden`
  passed.
- `compile_commands.json` was generated with 3,307 lines.
- generated dictionaries matched committed dictionary baselines.
- CMake-staged one-shot US English WAV output matched exactly for speakers 0
  through 8.
- CMake-staged expanded US audio suites matched exactly for speakers 0 through
  8.
- CMake-staged `libtts.so` matched the committed exact exported-symbol
  baseline.
- CMake language-library exported symbol name/type sets matched committed
  baselines.
- `dt_platform_smoke` passed with default audio metadata:
  `audio_disabled=0`, `audio_oss=1`, `audio_alsa=0`,
  `audio_pulseaudio=0`, and `audio_audioqueue=0`.
- `opthread_smoke` passed with the current legacy `OP_*` smoke semantics.

Manifest evidence:

- current Autotools staged path/type manifest:
  `baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest.txt`
- current CMake staged path/type manifest:
  `baseline-runs/next3-phase12-cmake-readiness/dist-manifest.txt`
- path/type comparison:
  `tools/baseline/compare_manifest.sh --expected baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest.txt --actual baseline-runs/next3-phase12-cmake-readiness/dist-manifest.txt --out baseline-runs/next3-phase12-cmake-basic-vs-autotools.diff`
- result: `manifest: ok`, with 589 entries on each side.
- detailed CMake-vs-Autotools comparison:
  `baseline-runs/next3-phase12-cmake-detailed-vs-autotools.diff`
- detailed result: `manifest: different`, with 1,126 entries on each side.

Promotion blockers:

- CMake-built binaries still have different sizes and SHA-256 hashes from
  Autotools-built binaries.
- `doc/DECtalk/html` directory metadata still differs.
- CMake language-library full symbol address/order captures are not accepted as
  exact parity, although name/type sets match.
- CMake audio options are modeled as macro state only; live-audio probing,
  backend linkage, device selection, callback timing, queue behavior, and live
  hardware behavior remain outside verified coverage.

Promotion should be handled by a separate future plan. This phase does not
promote CMake and does not remove Autotools or any existing build path.

## Phase 18 Promotion Recommendation

This historical recommendation predates the later Phase 5 packaging closure. See
the current `Remaining Gaps` section for the latest staged-layout status.

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

## Phase 10 Reassessment

This historical reassessment predates the later Phase 5 packaging closure. See
the current `Remaining Gaps` section for the latest staged-layout status.

Recommendation: do not promote CMake to the primary Linux build path yet.
Autotools remains authoritative.

Current evidence from
`tools/baseline/verify_cmake_subset.sh --run-dir baseline-runs/next-phase10-cmake --expected tests/golden`:

- CMake configures and builds the side-by-side subset.
- `compile_commands.json` is generated and non-empty.
- generated CMake dictionaries match `tests/golden/dictionaries` exactly.
- CMake-staged `say` produces exact original US English golden WAVs for
  speakers 0 through 8.
- CMake-staged `say` also produces exact expanded US audio suite WAVs.
- CMake-staged `libtts.so` matches the committed exported-symbol baseline
  exactly.
- CMake language-library exported symbol name/type sets match the committed
  baselines.
- Ubuntu CI runs `tools/baseline/verify_cmake_subset.sh`.

Promotion blockers:

- CMake still does not stage all intended Linux artifacts. The current detailed
  manifest comparison has 589 Autotools paths, 530 CMake paths, 59 missing
  Autotools paths, and 0 extra CMake paths.
- Remaining missing paths are still the unbuilt `aclock` and `dtmemory`
  top-level tools, generated sample text files under `src/DECtalk/dtsamples/`,
  helper/user-dictionary tools under `tools/`, and staged `/usr/bin` symlinks.
- CMake does not expose the live audio backend options used by the Autotools
  runtime build.
- CMake packaging parity is not proven, and the remaining layout differences
  have not been explicitly accepted as final.

Next CMake work should close the staged-layout gap or document explicit
acceptance for each remaining difference. Promotion should remain a separate
decision after exact or accepted packaging parity is available.

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
