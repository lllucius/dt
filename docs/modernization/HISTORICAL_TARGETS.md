# Historical Targets

Linux is the current intended modernization target. Historical and non-current
targets remain in the tree for inventory and preservation, but they should not
accidentally shape Linux cleanup work.

## Quarantine status

- Existing Autotools, Visual Studio, and source preprocessor branches were not
  removed or rewritten.
- The initial side-by-side CMake build is explicitly Linux-only by default.
- Non-Linux CMake experimentation requires the opt-in option
  `DECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON`.
- Autotools configuration now requires `--enable-legacy-targets` for
  historical/non-current target triplets before target-specific branches are
  selected. Current non-iPAQ Linux configuration remains the default path.
- Platform scaffolding includes `src/platform/dt_legacy_targets.*`, which
  reports historical target macros and requires
  `DECTALK_ENABLE_LEGACY_TARGET_SOURCE` when those macros are active.

This is a build-system and scaffolding-level quarantine. It does not claim that
historical source branches are deleted, dead, validated, or safe to remove.

## Source-level opt-in names

- Autotools: `./configure --enable-legacy-targets`
- CMake: `-DDECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON`
- Source scaffolding macro: `DECTALK_ENABLE_LEGACY_TARGET_SOURCE`

## Historical or non-current targets to preserve

- Windows.
- Visual Studio 6.
- Visual Studio 2022.
- macOS.
- iOS-related Apple history.
- OSF/Tru64.
- VxWorks.
- MS-DOS and DOS file-format handling.
- Windows CE.
- Solaris/SPARC.
- ARM7.
- MIPS.
- old PowerPC Mac.
- iPAQ Linux.
- Emscripten/WebAssembly.

## Observed indicators

- Windows: `WIN32`, `_WIN32`, Visual Studio `.sln`, `.vcxproj`, `.dsp`, `.dsw`,
  and `devops/vs*` batch scripts.
- Windows CE: `UNDER_CE`, `_WIN32_WCE_EMULATION`, `src/samplece`, and Windows CE
  installation assets.
- OSF/Tru64: `__osf__`, `OSF_VERSION_V3.2`, `OSF_VERSION_V4.0`, `src/docsosf`,
  `src/kitosf`, and `src/dapi/src/osf`.
- Solaris/SPARC: `_SPARC_SOLARIS_`, `_BIGENDIAN_`, and `sparc-sun-*` configure
  branches.
- iPAQ Linux: `arm-ipaq-linux-*`, `__ipaq__`, and `/skiff/local` cross-toolchain
  paths.
- ARM7: `ARM7`, `EPSON_ARM7`, and `src/dapi/src/epsonarm7*`.
- PowerPC Mac: `_APPLE_MAC_`, `powerpc-apple-*`, and big-endian branches.
- macOS/iOS: `__APPLE__`, Apple configure branches, and README/CI references.
- Emscripten: `__EMSCRIPTEN__` and `ports/emscripten`.
- MS-DOS: `MSDOS`, DOS dictionary file handling, and DOS code-page comments.
- VxWorks: `VXWORKS` conditionals.
- MIPS: listed as historical in project guidance; no Phase 8 source quarantine
  was attempted.

## Rules before deeper quarantine

1. Capture or update Linux behavior baselines before changing shared code.
2. Classify each macro before changing it.
3. Do not remove historical code without explicit approval.
4. Prefer new build-system options and documentation before source movement.
5. Avoid changing speech, dictionary, threading, audio, or public API behavior as
   part of historical-target cleanup.

## Phase 11 quarantine audit

Phase 11 did not move, delete, or rewrite historical target code. It reviewed
which historical target groups still affect modernization planning.

### Current quarantine controls

- Autotools blocks historical/non-current target configuration unless
  `--enable-legacy-targets` is supplied.
- CMake blocks non-Linux configuration unless
  `-DDECTALK_CMAKE_ENABLE_LEGACY_TARGETS=ON` is supplied.
- `src/platform/dt_legacy_targets.*` reports historical target macro state and
  requires `DECTALK_ENABLE_LEGACY_TARGET_SOURCE` when those macros are active
  inside the platform scaffolding.

These controls are sufficient for current Linux modernization work. They are
not evidence that historical target branches build or behave correctly.

### Blockers by modernization area

- CMake promotion: Apple, Emscripten, Solaris/SPARC, Windows, OSF/Tru64,
  VxWorks, iPAQ, ARM7, and PowerPC branches still require explicit opt-in. CMake
  should not be promoted beyond Linux until each target is either separately
  supported, quarantined as unsupported, or excluded by policy.
- Warning cleanup: platform-specific warning edits under shared files must be
  treated as historical-target work when they involve `WIN32`, `UNDER_CE`,
  `__APPLE__`, `_APPLE_MAC_`, `__osf__`, `_SPARC_SOLARIS_`, `VXWORKS`,
  `MSDOS`, `ARM7`, `EPSON_ARM7`, `__ipaq__`, `__EMSCRIPTEN__`, `ALPHA`, or
  endian branches.
- Platform wrappers: no runtime wrapper should claim parity for historical
  targets. Current wrapper evidence is Linux/POSIX scaffolding only and does not
  validate legacy audio, callback, queue, thread-priority, stack-size, or event
  behavior.

### Future quarantine options

- Add per-target documentation pages only after a target-specific inventory is
  needed. Start with source ownership, build entry points, macro list, and the
  reason the target remains non-current.
- Keep build-system opt-ins additive. Do not make historical target macros
  active in default Linux builds.
- If a future plan proposes deleting or moving a historical branch, require an
  explicit approval item, a list of affected files, and a statement that Linux
  behavior gates still pass.
- If a future plan proposes reviving a historical target, treat it as new target
  support rather than cleanup and define separate build, packaging, audio,
  dictionary, and API gates for that target.
