# Phoneme Baseline Feasibility

This document records Phase 3 of the next modernization plan. It inventories
public-facing phoneme and text-mode capture paths for deterministic regression
coverage. It does not accept new golden outputs and does not approve parser,
phoneme, LTS, synthesis, API, dictionary, or runtime behavior changes.

## Scope

Current target:

- Linux
- US English only for probes
- no live audio hardware
- no source, dictionary, public header, exported symbol, or golden output
  changes

Probe artifacts were written only under ignored
`baseline-runs/next2-phase3-feasibility/`.

## Candidate Paths

### `TextToSpeechConvertToPhonemes`

The installed Linux library exports `TextToSpeechConvertToPhonemes`, and the
installed public header declares it. The implementation writes ARPABET phoneme
text into an application-provided buffer and supports `TTS_SILENT` to suppress
audio sample output during conversion.

Observed status:

- candidate is public API and no-audio capable in design
- a temporary probe compiled successfully against `dist/include` and
  `dist/lib`
- the probe crashed at runtime when calling the exported API from the staged
  Linux install

Decision:

- do not accept a `TextToSpeechConvertToPhonemes` golden baseline in this phase
- treat any attempt to stabilize this path as a later extra-high API-boundary
  task because it touches exported API behavior near phoneme/LTS output

### `TextToSpeechOpenLogFile(..., LOG_PHONEMES)`

The public API documents log-file mode for text, phoneme, and syllable output.
The implementation accepts `LOG_PHONEMES`, and the Windows-style sample under
`src/samples/SAY/say.c` includes a `-lp` phoneme logging option.

Observed status:

- the Linux `dist/say` program is built from the OSF sample path and does not
  expose the `-lp` option
- a temporary public-API probe compiled successfully against `dist/include` and
  `dist/lib`
- `TextToSpeechOpenLogFile(..., LOG_PHONEMES)` returned `MMSYSERR_ERROR` in
  the staged Linux no-audio setup used by baseline tools
- inline `[:log phonemes on]` with `dist/say -fo` produced WAV output but did
  not create a separate phoneme log artifact in the probe directory

Decision:

- do not accept a log-file phoneme golden baseline in this phase
- defer until a no-audio public logging path is proven reliable or explicitly
  repaired under an extra-high API/runtime checkpoint

### `[:phoneme on]` Command Text

The command parser supports phoneme interpretation mode, and `dist/say -h`
documents `-pre "[:phoneme on]"` and `-post "[:phoneme off]"`.

Observed status:

- this command changes how input is interpreted for speech generation
- it does not by itself provide a separate deterministic text artifact suitable
  for CI comparison

Decision:

- do not use `[:phoneme on]` as a phoneme baseline capture path
- keep existing WAV comparisons as the deterministic behavior gate for command
  text and parser-facing coverage

### In-Memory API Phoneme Arrays

The public in-memory output API can return `TTS_PHONEME_T` arrays through
`TTS_BUFFER_T`. Existing `dtmemory` samples demonstrate this general API
surface.

Observed status:

- this is public API, but it captures callback-style timing/sample metadata
  rather than a simple public-facing text artifact
- it is better aligned with the future public API smoke matrix than with a
  parser-facing phoneme text baseline

Decision:

- defer to Phase 4 public API smoke matrix design
- keep comparisons limited to stable scalar values unless extra-high approval
  accepts timing-sensitive phoneme array expectations

## Recommendation

No new phoneme or text-mode golden baseline should be accepted from Phase 3.

The lowest-risk next step is to expand deterministic public API smoke coverage
first, then revisit phoneme capture only after the API matrix can distinguish
startup, no-audio output state, log-file mode, in-memory mode, and shutdown
behavior. Any accepted future phoneme baseline should require extra-high
approval and should include:

- exact fixture text
- exact language and speaker selection
- exact public API or command surface
- exact output artifact format
- byte-for-byte comparison when textual, or explicit stable-field comparison
  when structured
- unchanged audio, dictionary, public header, exported symbol, manifest, API
  smoke, and warning-budget gates

## Verification

Commands and observations:

```sh
dist/say -h
nm -D dist/lib/libtts.so | rg 'TextToSpeech(ConvertToPhonemes|OpenLogFile|CloseLogFile)'
gcc -I/home/yam/dt/dist/include baseline-runs/next2-phase3-feasibility/phoneme_probe.c \
  -L/home/yam/dt/dist/lib -Wl,-rpath,/home/yam/dt/dist/lib -ltts \
  -o baseline-runs/next2-phase3-feasibility/phoneme_probe
gcc -I/home/yam/dt/dist/include baseline-runs/next2-phase3-feasibility/log_phoneme_probe.c \
  -L/home/yam/dt/dist/lib -Wl,-rpath,/home/yam/dt/dist/lib -ltts \
  -o baseline-runs/next2-phase3-feasibility/log_phoneme_probe
```

Results:

- `dist/say -h` did not list `-lp`; it listed `-fo`, `-fi`, `-a`, language
  selection, speaker/rate/volume options, and `-pre`/`-post` command injection.
- `libtts.so` exports `TextToSpeechConvertToPhonemes`,
  `TextToSpeechOpenLogFile`, and `TextToSpeechCloseLogFile`.
- The `TextToSpeechConvertToPhonemes` probe compiled, then exited with a
  segmentation fault in the staged Linux runtime.
- The `LOG_PHONEMES` probe compiled, then returned
  `TextToSpeechOpenLogFile=1`.
- `dist/say -fo inline-log.wav -a "[:log phonemes on] Hello, world. [:log phonemes off]"`
  completed, but produced only the WAV artifact in the probe directory.

Because this phase changed documentation only, final verification was limited
to whitespace checking:

```sh
git diff --check -- . ':(exclude)src/dapi/src/cmd/cm_cmd.c'
```

## Next High-Risk Plan Phase 3 Refresh

The post-PR #5 plan rechecked public-facing phoneme and text output capture
paths under `baseline-runs/next4-phase3-phoneme-text/`. Temporary probes were
kept under `baseline-runs/` and were not committed as source, tooling, or
accepted fixtures.

Inspection:

```sh
dist/say -h
nm -D dist/lib/libtts.so | \
  rg 'TextToSpeech(ConvertToPhonemes|OpenLogFile|CloseLogFile|OpenInMemory|ReturnBuffer|AddBuffer)'
```

Observed status:

- `dist/say -h` still exposes `-fo`, `-fi`, `-a`, `-l`, `-pre`, and `-post`,
  but it does not expose the Windows sample `-lp` phoneme log option.
- `libtts.so` still exports `TextToSpeechConvertToPhonemes`,
  `TextToSpeechOpenLogFile`, `TextToSpeechCloseLogFile`,
  `TextToSpeechOpenInMemory`, `TextToSpeechAddBuffer`, and
  `TextToSpeechReturnBuffer`.

Probe results from the correct installed runtime context:

- Running the probes from the repository root failed at startup because the
  staged runtime could not locate `dtalk_us.dic`; rerunning from `dist/`
  matched the existing API smoke setup.
- `TextToSpeechConvertToPhonemes` with `TTS_SILENT` segfaulted twice when run
  from `dist/`.
- `TextToSpeechOpenLogFile(..., LOG_TEXT)`,
  `TextToSpeechOpenLogFile(..., LOG_PHONEMES)`, and
  `TextToSpeechOpenLogFile(..., LOG_SYLLABLES)` all returned
  `MMSYSERR_ERROR` in the no-audio setup.
- A no-live-audio WAV-output variant was also probed. With relative WAV output,
  `TextToSpeechOpenWaveOutFile` could be opened, but
  `TextToSpeechOpenLogFile(..., LOG_TEXT)` still returned `MMSYSERR_ERROR`.
- Inline command probes using `[:log text on]`, `[:log phonemes on]`, and
  `[:log syllables on]` through `dist/say -fo` generated WAV files but did not
  create a separate text, phoneme, syllable, or `log.txt` artifact in the probe
  directories.
- `tools/baseline/check_api_smoke.sh --out
  baseline-runs/next4-phase3-api-smoke-check` passed, confirming the installed
  public API smoke and deterministic WAV path still work.

Decision:

- Do not add phoneme or text golden fixtures in this phase.
- Do not add capture or compare scripts yet, because there is no proven
  deterministic public artifact to compare.
- Defer phoneme/text baselines until either the public phoneme conversion crash
  is understood and fixed under an API-boundary gate, or the public log-file
  path can create a stable no-live-audio text artifact.

Behavior statement: this refresh did not change parser, phoneme, LTS,
synthesis, timing, dictionary, public API, audio generation, source, build
scripts, golden fixtures, or accepted baselines.
