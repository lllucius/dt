# Audio Backend Containment

This document records the current audio backend behavior before any routing is
introduced through platform abstractions. It is an inventory and containment
checkpoint, not a behavior-change proposal.

## Current Implementation

Live audio output is implemented in `src/dapi/src/nt/linux_audio.c`. Despite the
file name, it also contains legacy Solaris, VxWorks, and Apple AudioQueue
conditionals. For the current Linux target, the file defines `USE_OSS`
internally and uses `/dev/dsp` as the default legacy device path.

When compiled in, `wodOpen` attempts backends in this order:

1. PulseAudio simple API when `USE_PULSEAUDIO` is defined.
2. ALSA when `USE_ALSA` is defined.
3. OSS through `/dev/dsp` on Linux or `/dev/audio` on Solaris.
4. AudioQueue on Apple builds.

The OSS path can read `DECtalk.conf` for an `AudioDev:` entry. If that entry
starts with `$`, the following text is treated as an environment variable name.
If no configured device is found, the compiled legacy device path is used.

ALSA uses the `ALSA_DEFAULT` environment variable when present, otherwise the
compiled device name `default`.

## Autotools Options

Autotools remains the authoritative build path for audio behavior.

- `./configure --disable-audio` defines `DISABLE_AUDIO` through
  `AUDIO_DEFINES`.
- `./configure --disable-pulseaudio` prevents PulseAudio probing even when
  `libpulse-simple` development files are installed.
- When audio is enabled, `configure` probes `libpulse-simple` and ALSA with
  `pkg-config` and adds `USE_PULSEAUDIO`, `USE_ALSA`, and the corresponding
  link flags when available.
- Apple framework link flags are still present as historical/non-current target
  support and are not part of the current Linux target.

## CMake Status

The side-by-side CMake build exposes opt-in audio macro options:

- `DECTALK_CMAKE_DISABLE_AUDIO`
- `DECTALK_CMAKE_USE_ALSA`
- `DECTALK_CMAKE_USE_PULSEAUDIO`

The defaults preserve the current verified CMake state:
`audio_disabled=0`, `audio_oss=1`, `audio_alsa=0`, and
`audio_pulseaudio=0`. The options model the Autotools macro surface, but they
do not replace Autotools probing or prove live-audio behavior.

Phase 11 adds `src/platform/dt_audio_backend.*` only as compile-time metadata
scaffolding for CMake smoke checks. It reports the backend macros and legacy
device constants without opening audio devices and without linking audio
libraries. It is not wired into `linux_audio.c`, Autotools, installed headers,
or runtime libraries.

## Deterministic Output

No-hardware WAV generation remains the preferred verification path. The baseline
scripts drive `say` with file output and compare committed golden WAVs for all
nine US English speakers. Phase 11 must keep those comparisons exact.

## Containment Rules

- Do not change sample rate.
- Do not change callback timing.
- Do not change default backend selection.
- Do not route runtime audio through `src/platform` wrappers until a future
  phase explicitly approves it and exact baselines still pass.
- Keep `linux_audio.c` as the authoritative implementation until a wrapper can
  delegate to it without behavior drift.

## Accelerated Phase 10 CMake Option Evidence

Phase 10 added CMake cache options for the audio macro surface while preserving
the default verified behavior.

Default CMake subset verification:

```sh
tools/baseline/verify_cmake_subset.sh \
  --run-dir baseline-runs/next3-phase10-cmake-audio-options \
  --expected tests/golden
```

Default `dt_platform_smoke` audio metadata:

```text
audio_disabled=0
audio_oss=1
audio_alsa=0
audio_pulseaudio=0
audio_audioqueue=0
audio_legacy_oss_device=/dev/dsp
```

Metadata-only disabled-audio probe:

```sh
cmake -S . -B baseline-runs/next3-phase10-cmake-disable-audio/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DDECTALK_CMAKE_DISABLE_AUDIO=ON \
  -DCMAKE_C_COMPILER=/usr/bin/gcc
cmake --build baseline-runs/next3-phase10-cmake-disable-audio/build \
  --target dt_platform_smoke -- -j1
baseline-runs/next3-phase10-cmake-disable-audio/build/dt_platform_smoke \
  /home/yam/dt PLAN.md
```

Disabled-audio metadata:

```text
audio_disabled=1
audio_oss=0
audio_alsa=0
audio_pulseaudio=0
audio_audioqueue=0
```

The disabled-audio probe validates metadata visibility only. It does not
exercise live devices, promote CMake, or approve runtime audio behavior changes.

## Next High-Risk Plan Phase 9 Option Matrix

Phase 9 added a repeatable audio-option metadata helper:

```sh
tools/baseline/check_audio_option_matrix.sh \
  --run-dir baseline-runs/next4-phase9-audio-option-matrix
```

The helper does not open live audio devices. CMake rows build and run only
`dt_platform_smoke`; Autotools rows run `configure` and capture generated audio
macro/link metadata from Makefiles.

Local dependency evidence:

```text
alsa: not found
libpulse-simple: not found
```

CMake metadata results:

| Probe | Classification | disabled | OSS | ALSA | PulseAudio | AudioQueue |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| default | metadata-only | 0 | 1 | 0 | 0 | 0 |
| `DECTALK_CMAKE_DISABLE_AUDIO=ON` | metadata-only | 1 | 0 | 0 | 0 | 0 |
| `DECTALK_CMAKE_USE_ALSA=ON` | metadata-only | 0 | 1 | 1 | 0 | 0 |
| `DECTALK_CMAKE_USE_PULSEAUDIO=ON` | metadata-only | 0 | 1 | 0 | 1 | 0 |

Each CMake probe generated `compile_commands.json` with 3,307 lines and
reported the legacy Linux OSS device as `/dev/dsp`. The ALSA and PulseAudio
CMake rows are metadata-only because the local development packages were not
available and the CMake options do not replace Autotools probing or link those
backend libraries.

Autotools metadata results:

| Probe | Classification | Captured audio metadata |
| --- | --- | --- |
| default | configure-metadata-only | `LINUX_AUDIO=$(OUTPUT_DIR)/linux_audio.o`; no `AUDIO_DEFINES`; no `AUDIO_LIBS` |
| `--disable-audio` | configure-metadata-only | `AUDIO_DEFINES=-DDISABLE_AUDIO`; no `AUDIO_LIBS` |
| `--disable-pulseaudio` | configure-metadata-only | `LINUX_AUDIO=$(OUTPUT_DIR)/linux_audio.o`; no `AUDIO_DEFINES`; no `AUDIO_LIBS` |

Default live-audio behavior remains unchanged by this phase. The deterministic
default gates remain the behavior evidence: no-hardware WAV output, API smoke,
callback smoke, dictionaries, symbols, manifests, and warning budgets are still
verified by the default Autotools baseline gate; CMake default dictionaries,
US audio, symbols, platform smoke, and OP smoke remain verified by the CMake
subset gate.
