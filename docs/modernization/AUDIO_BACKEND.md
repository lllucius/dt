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

The side-by-side CMake build does not currently expose live audio backend
options. It builds the current Linux source set with default defines and does
not pass `USE_ALSA`, `USE_PULSEAUDIO`, or `DISABLE_AUDIO` into the DECtalk
runtime targets.

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
