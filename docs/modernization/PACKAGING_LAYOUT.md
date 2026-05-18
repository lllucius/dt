# Packaging And Install Layout

This document records the current Linux Autotools packaging layout and the
manifest checks used to keep install changes visible.

## Authoritative Layout

Autotools remains authoritative for the installed `dist/` tree. The current
`src/Makefile.in` `install` target creates these top-level areas:

- `DECtalk.conf`
- `README`
- `say`, `aclock`, `dtmemory`, optional `gspeak`, and optional `windic`
- `bitmaps/`
- `dic/`
- `doc/DECtalk/`
- `include/dtk/`
- `lib/`
- `src/DECtalk/`
- `tools/`
- `/usr/bin` symlinks under the staged `DESTDIR`

The install rule also generates `DECtalk.conf` at install time. Dictionary,
language, bitmap, and sample-source paths in that file are part of the runtime
contract and should not be edited without behavior verification.

## Manifest Modes

`tools/baseline/capture_dist_manifest.sh` supports three formats:

- `basic`: path, file type, and symlink target. This remains the default for
  compatibility with earlier accepted captures.
- `metadata`: path, file type, mode, size, and symlink target.
- `metadata-hash`: metadata plus SHA-256 hashes for regular files.

Use the detailed modes when packaging work might affect file permissions,
symlink targets, generated config contents, or payload bytes.

The accepted detailed Linux Autotools manifest is committed at:

- `tests/golden/dist-manifest-detailed.txt`

Regenerate and compare with:

```sh
tools/baseline/capture_dist_manifest.sh \
  --format metadata-hash \
  --out baseline-runs/current/dist-manifest-detailed.txt
tools/baseline/compare_manifest.sh \
  --expected tests/golden/dist-manifest-detailed.txt \
  --actual baseline-runs/current/dist-manifest-detailed.txt \
  --out baseline-runs/current/dist-manifest-detailed.diff
```

## CMake Status

The side-by-side CMake staged layout now has exact basic path/type parity with
the Autotools staged layout. After the Phase 5 packaging update, the basic
manifest comparison shows 589 Autotools entries, 589 CMake entries, no missing
paths, and no extra paths.

Do not promote CMake packaging until detailed manifest comparisons are either
exact or each difference is explicitly reviewed and accepted.

The detailed metadata-hash manifest is still intentionally not accepted as
equivalent. The CMake and Autotools detailed manifests both contain 1,126 lines,
but CMake-built binaries have different sizes and hashes, and the
`doc/DECtalk/html` directory metadata differs. These are not path/type omissions,
but they remain promotion blockers until explicitly reviewed.

## Accelerated Plan Phase 6 Policy

The accelerated Phase 6 detailed comparison still reports
`manifest: different` for CMake versus Autotools:

- Autotools detailed manifest:
  `baseline-runs/next3-phase1-post-merge/dist-manifest-detailed.txt`
- CMake detailed manifest:
  `baseline-runs/next3-phase1-post-merge-cmake/dist-manifest-detailed.txt`
- comparison diff:
  `baseline-runs/next3-phase6-cmake-detailed-vs-autotools.diff`

Acceptance policy:

- path/type parity is accepted for the current side-by-side CMake stage.
- detailed binary size/hash differences are not accepted as full packaging
  parity.
- the `doc/DECtalk/html` directory metadata difference is not accepted as full
  metadata parity.
- future CMake promotion must either eliminate these differences or explicitly
  approve each remaining difference class with matching release and behavior
  verification.

## Accelerated Plan Phase 12 CMake Packaging Decision

Phase 12 rechecked CMake packaging after the API-boundary warning pilot and the
Autotools detailed manifest refresh.

Current path/type evidence:

- Autotools manifest:
  `baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest.txt`
- CMake manifest:
  `baseline-runs/next3-phase12-cmake-readiness/dist-manifest.txt`
- comparison output:
  `baseline-runs/next3-phase12-cmake-basic-vs-autotools.diff`
- result: `manifest: ok`, with 589 entries in each manifest.

Current detailed evidence:

- Autotools detailed manifest:
  `baseline-runs/next3-phase11-api-boundary-services-lf/dist-manifest-detailed.txt`
- CMake detailed manifest:
  `baseline-runs/next3-phase12-cmake-readiness/dist-manifest-detailed.txt`
- comparison output:
  `baseline-runs/next3-phase12-cmake-detailed-vs-autotools.diff`
- result: `manifest: different`, with 1,126 entries in each manifest.

Decision: CMake remains side-by-side. Basic path/type staging parity is still
accepted, but detailed metadata/hash differences for built binaries and
`doc/DECtalk/html` directory metadata remain release-packaging promotion
blockers.
