# Public API And Export Audit

This audit records the current Linux public header and exported-symbol
boundaries. It does not approve public API changes.

## Installed Headers

Autotools installs these headers under `include/dtk/`:

- `src/dapi/src/api/ttsapi.h`
- `src/dapi/src/osf/dtmmedefs.h`
- `src/dapi/src/include/l_all_ph.h`
- `src/dapi/src/include/l_com_ph.h`
- `src/dapi/src/include/l_fr_ph.h`
- `src/dapi/src/include/l_gr_ph.h`
- `src/dapi/src/include/l_la_ph.h`
- `src/dapi/src/include/l_sp_ph.h`
- `src/dapi/src/include/l_uk_ph.h`
- `src/dapi/src/include/l_us_ph.h`

`src/dapi/src/api/tts.h` is not installed by the current Linux Autotools install
target, but it remains ABI-sensitive internal API surface because it is included
by runtime sources and listed in modernization risk areas.

## Header Isolation

`tools/baseline/check_public_headers.sh` verifies the installed header list and
syntax-checks headers that currently self-compile on Linux:

- `ttsapi.h`
- `tts.h`
- `dtmmedefs.h`
- `l_com_ph.h`
- `l_fr_ph.h`

The remaining installed phoneme headers are inventoried but not forced through
the isolation check because they intentionally contain `#error` guards or
require voice-ROM selection macros. They should not be rewritten as part of this
audit.

## Exported Symbols

Exported symbols are versioned under `tests/golden/symbols/` and compared by
`tools/baseline/compare_symbols.sh`. Any difference in those files is
API/ABI-relevant until explicitly reviewed and accepted.

## API Smoke Matrix

`tools/baseline/check_api_smoke.sh` compiles
`tools/baseline/api_smoke.c` against installed `dist/include` headers and
`dist/lib` libraries, then runs from `dist/` so `DECtalk.conf`, dictionaries,
and relative library loading match the installed Linux layout.

Current deterministic coverage:

- error-safe `TextToSpeechShutdown(NULL)` return handling
- `TextToSpeechEnumLangs()` for the six installed language entries from
  `DECtalk.conf`
- `TextToSpeechVersion()`, `TextToSpeechGetFeatures()`, and
  `TextToSpeechVersionEx()`
- `TextToSpeechStartLang("us")` and default `TextToSpeechSelectLang()`
- `TextToSpeechGetCaps()` stable scalar fields
- no-audio `TextToSpeechStartup()`
- get/set/error cases for rate, speaker, language, and main volume, with state
  restored before audio generation
- second no-audio `TextToSpeechStartup()`/`TextToSpeechShutdown()` lifecycle
  check before the main handle is created
- `TextToSpeechGetStatus(INPUT_CHARACTER_COUNT)`
- deterministic no-audio status/error cases for null rate/speaker output
  pointers, zero-count status requests, `STATUS_SPEAKING`,
  `WAVE_OUT_DEVICE_ID`, mixed no-audio status requests,
  `TextToSpeechCloseInMemory()` when memory output is not open, and
  `TextToSpeechOpenInMemory()` with an invalid format
- WAV-file output through `TextToSpeechOpenWaveOutFile()`,
  `TextToSpeechSpeak()`, `TextToSpeechSync()`,
  `TextToSpeechCloseWaveOutFile()`, and `TextToSpeechShutdown()`
- byte-for-byte comparison against the accepted US English speaker 0 WAV

Explicitly unsupported in the smoke matrix:

- live audio hardware
- callback ordering or timing
- in-memory output buffers and phoneme arrays
- phoneme/text log capture
- non-US speech output
- thread lifecycle, queue, pipe, or audio-backend behavior
- full public API conformance

## Policy

- Do not reformat public headers.
- Do not change public function signatures, exported symbol names, structure
  layouts, constants, calling conventions, or installed header paths without an
  explicit API/ABI decision.
- Treat generated symbol comparisons and header audit comparisons as blocking
  checks for public API work.

## Accelerated Plan Phase 7 Update

Phase 7 expanded `tools/baseline/api_smoke.c` without changing public headers
or runtime implementation files.

Added coverage:

- second no-audio startup/shutdown cycle
- null output-pointer checks for `TextToSpeechGetRate()` and
  `TextToSpeechGetSpeaker()`
- `TextToSpeechGetStatus()` zero-count validation
- no-audio `STATUS_SPEAKING` and `WAVE_OUT_DEVICE_ID` status behavior
- mixed no-audio status behavior for input count plus speaking state
- `TextToSpeechCloseInMemory()` when memory output is not open
- `TextToSpeechOpenInMemory()` invalid-format validation

Verification:

```sh
tools/baseline/check_api_smoke.sh \
  --out baseline-runs/next3-phase7-api-smoke/api-smoke
tools/baseline/verify_current.sh \
  --run-dir baseline-runs/next3-phase7-api-smoke \
  --expected tests/golden
```

Results:

- expanded API smoke output was deterministic.
- API smoke WAV comparison remained exact against
  `tests/golden/audio/us/speaker_0.wav`.
- public headers, exported symbols, dictionaries, user dictionaries, detailed
  Autotools manifest, one-shot US audio, expanded US audio suites, and warning
  budgets matched accepted baselines.
