# Baseline Procedure

Phase 1 captures current Linux behavior before cleanup. Full run artifacts are
kept under ignored `baseline-runs/phase1/`; only the golden input and WAV files
are committed.

## Build baseline

```sh
mkdir -p baseline-runs/phase1
(cd src && autoreconf -i) > baseline-runs/phase1/autoreconf.log 2>&1
(cd src && CC=/usr/bin/gcc ./configure) > baseline-runs/phase1/configure.log 2>&1
(cd src && make) > baseline-runs/phase1/build.log 2>&1
```

The Phase 1 default build succeeded.

## Strict warning inventory

```sh
(cd src && make clean) > baseline-runs/phase1/clean-before-strict.log 2>&1
(cd src && make WARN_FLAGS='-Wall -Wextra -Wpedantic -Wformat=2 -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-declarations -Wpointer-arith -Wcast-qual -Wwrite-strings -Wbad-function-cast -Wnested-externs') > baseline-runs/phase1/build-strict-warnings.log 2>&1
```

Observed warning-line counts:

- Default build: 1,835.
- Strict warning rebuild: 29,860.

The strict rebuild completed successfully. The largest strict warning categories
were signedness mismatches, discarded qualifiers, missing braces around
initializers, and volatile/const qualifier issues.

## Dist manifest

```sh
find dist -maxdepth 4 -printf '%y %p -> %l\n' | sort > baseline-runs/phase1/dist-manifest.txt
```

The manifest records path and file type only.

## Exported symbols

```sh
for lib in dist/lib/*.so; do
  base=$(basename "$lib")
  nm -D --defined-only "$lib" | sort > "baseline-runs/phase1/symbols-${base}.txt"
done
```

Phase 1 captured symbols for all shared libraries installed into `dist/lib/`:

- `libtts.so`
- `libtts_fr.so`
- `libtts_gr.so`
- `libtts_la.so`
- `libtts_sp.so`
- `libtts_uk.so`
- `libtts_us.so`

## Golden audio

Committed input:

- `tests/golden/input/us_one_shot.txt`

Committed WAV outputs:

- `tests/golden/audio/us/speaker_0.wav`
- `tests/golden/audio/us/speaker_1.wav`
- `tests/golden/audio/us/speaker_2.wav`
- `tests/golden/audio/us/speaker_3.wav`
- `tests/golden/audio/us/speaker_4.wav`
- `tests/golden/audio/us/speaker_5.wav`
- `tests/golden/audio/us/speaker_6.wav`
- `tests/golden/audio/us/speaker_7.wav`
- `tests/golden/audio/us/speaker_8.wav`

Generation command pattern:

```sh
(cd dist && LD_LIBRARY_PATH=lib ./say -l us -s 0 -fi ../tests/golden/input/us_one_shot.txt -fo ../tests/golden/audio/us/speaker_0.wav)
```

Repeat with speaker IDs `0` through `8`. The Phase 1 WAV files are RIFF/WAVE,
PCM, 16-bit, mono, 11025 Hz.

## Limitations

- Baseline audio covers US English only.
- Baseline audio covers one fixed input text and speakers 0 through 8.
- No live audio hardware path was tested.
- Optional GTK, ALSA, and PulseAudio development packages were not available on
  the host used for Phase 1.
- Captured outputs are a baseline for comparison; they are not by themselves a
  proof that future behavior is preserved.
