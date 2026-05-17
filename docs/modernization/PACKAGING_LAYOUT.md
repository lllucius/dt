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

The side-by-side CMake staged layout is still a verification subset. It does not
yet claim packaging parity with Autotools. Phase 4 of the follow-on plan stages
low-risk static README, bitmap, documentation, and selected sample-source
assets. Current known gaps still include generated sample text files,
`/usr/bin` symlinks, and additional sample or helper tools.

Do not promote CMake packaging until detailed manifest comparisons are either
exact or each difference is explicitly reviewed and accepted.

After the Phase 4 CMake staging update, detailed manifest comparison shows 59
Autotools paths still missing from the CMake staged subset and no extra CMake
paths. Remaining gaps require additional build targets or generated-file/symlink
decisions and are intentionally deferred.
