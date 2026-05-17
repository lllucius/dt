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

Manifest comparisons can be run with:

```sh
tools/baseline/compare_manifest.sh --expected baseline-runs/accepted/dist-manifest.txt --actual baseline-runs/current/dist-manifest.txt
```

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

Symbol comparisons can be run with:

```sh
tools/baseline/compare_symbols.sh --expected tests/golden/symbols --actual baseline-runs/current/symbols
```

## Generated dictionaries

Dictionary hashes can be captured and compared with:

```sh
tools/baseline/capture_dictionaries.sh --out baseline-runs/current/dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries --actual baseline-runs/current/dictionaries
```

Generated dictionary changes are behavior-relevant until explicitly reviewed.

User dictionary fixture output can be captured and compared with:

```sh
tools/baseline/capture_user_dictionaries.sh --out baseline-runs/current/user-dictionaries
tools/baseline/compare_dictionaries.sh --expected tests/golden/dictionaries/user/expected --actual baseline-runs/current/user-dictionaries
```

User dictionary `.dtu` hash changes are also behavior-relevant until explicitly
reviewed.

## Golden audio

Committed input:

- `tests/golden/input/us_one_shot.txt`
- `tests/golden/input/us_abbreviations.txt`
- `tests/golden/input/us_commands_markup.txt`
- `tests/golden/input/us_punctuation_numbers.txt`

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

Audio comparisons can also emit a metrics report:

```sh
tools/baseline/compare_audio.py --actual baseline-runs/current/audio-us --metrics-out baseline-runs/current/audio-metrics.tsv
```

The metrics report includes SHA-256 hashes, frame counts, sample rates, peak and
RMS levels, and max sample deltas. Exact WAV equality remains the pass/fail
condition.

## Limitations

- Baseline audio covers US English only.
- Baseline audio covers one fixed input text and speakers 0 through 8.
- Additional committed input files expand future coverage, but they do not yet
  have committed WAV baselines.
- No live audio hardware path was tested.
- Optional GTK, ALSA, and PulseAudio development packages were not available on
  the host used for Phase 1.
- Captured outputs are a baseline for comparison; they are not by themselves a
  proof that future behavior is preserved.

## Single-command verification

`tools/baseline/verify_current.sh` runs the current build, capture, and available
comparison steps into one run directory:

```sh
tools/baseline/verify_current.sh --run-dir baseline-runs/current --expected baseline-runs/accepted
```

The expected directory is optional. When supplied, it may contain `symbols/`,
`dist-manifest.txt`, `dictionaries/`, and `dictionaries/user/expected/`
captures from a previously accepted run.
