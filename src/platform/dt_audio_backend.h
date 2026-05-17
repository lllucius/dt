/*
 * Purpose: internal audio backend metadata declarations for containment work.
 * Scope: private DECtalk modernization scaffolding; not a public API.
 * Behavior preservation: reports compile-time backend availability only and
 * does not open devices, change backend selection, or route runtime audio.
 * Limitations: mirrors the current Linux/POSIX backend macros for smoke checks;
 * linux_audio.c remains the authoritative live audio implementation.
 */

#ifndef DT_AUDIO_BACKEND_H
#define DT_AUDIO_BACKEND_H

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

typedef enum dt_audio_backend_id {
    DT_AUDIO_BACKEND_NONE = 0,
    DT_AUDIO_BACKEND_OSS = 1,
    DT_AUDIO_BACKEND_ALSA = 2,
    DT_AUDIO_BACKEND_PULSEAUDIO = 3,
    DT_AUDIO_BACKEND_AUDIOQUEUE = 4
} dt_audio_backend_id_t;

typedef struct dt_audio_backend_config {
    int disable_audio;
    int use_oss;
    int use_alsa;
    int use_pulseaudio;
    int use_audioqueue;
    const char *legacy_oss_device;
    const char *alsa_default_device;
    const char *selection_order;
} dt_audio_backend_config_t;

void dt_audio_backend_get_compile_config(dt_audio_backend_config_t *config);
const char *dt_audio_backend_name(dt_audio_backend_id_t backend);

DT_EXTERN_C_END

#endif
