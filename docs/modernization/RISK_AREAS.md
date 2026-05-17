# Risk Areas

The following areas are behavior-critical and should not be changed casually.
Phase 1 only records them so later work has explicit boundaries.

## Public API and ABI

- `src/dapi/src/api/ttsapi.h`
- `src/dapi/src/api/tts.h`
- `src/dapi/src/api/ttsapi.c`
- `src/dapi/src/api/init.c`
- Installed headers under `dist/include/dtk/`
- Exported symbols from all shared libraries in `dist/lib/`

## Speech and synthesis behavior

- `src/dectalkf.h`
- `src/dectalkf_klsyn.h`
- `src/dectalkf_hlsyn.h`
- `src/dapi/src/lts`
- `src/dapi/src/ph`
- `src/dapi/src/vtm`
- `src/dapi/src/hlsyn`
- Voice definitions, parser behavior, phoneme output, intonation, timing, and
  sample rate.

## Runtime behavior

- `src/dapi/src/nt/opthread.c`
- `src/dapi/src/nt/linux_audio.c`
- Audio callbacks, queues, pipe behavior, thread lifecycle, and timing.
- `TextToSpeechOpenWaveOutFile` paths, including WAV output and virtual stdout
  streams.

## Dictionaries and generated data

- Dictionary source text and tables under `src/dapi/src/dic`.
- Dictionary compiler outputs installed into `dist/dic/`.
- User dictionary tools under `src/udicunix/src`.
- Generated `.dic` files and dictionary lookup behavior.

## Packaging and install layout

- `src/Makefile.in` install target.
- `dist/` tree layout.
- `DECtalk.conf` keys and relative paths.
- Shared-library names and tool names.

## Phase 1 baseline artifacts

- Default build log: `baseline-runs/phase1/build.log`.
- Strict warning log: `baseline-runs/phase1/build-strict-warnings.log`.
- Dist manifest: `baseline-runs/phase1/dist-manifest.txt`.
- Exported symbols: `baseline-runs/phase1/symbols-*.txt`.
- Golden audio hashes: `baseline-runs/phase1/golden-audio-sha256.txt`.
- Committed golden WAV files: `tests/golden/audio/us/speaker_0.wav` through
  `tests/golden/audio/us/speaker_8.wav`.

These artifacts capture the current state. They do not prove behavior
preservation for future changes unless the same checks are rerun and compared.
