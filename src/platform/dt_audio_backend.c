/*
 * Purpose: compile-time audio backend metadata for platform containment.
 * Scope: private CMake smoke-test scaffolding only; not linked into DECtalk
 * runtime libraries by the authoritative Autotools build.
 * Behavior preservation: this file does not perform audio I/O and does not
 * replace or wrap linux_audio.c behavior.
 * Limitations: exposes macro-derived metadata so future backend abstraction can
 * be compared against the existing implementation before any routing change.
 */

#include "dt_audio_backend.h"

#if defined(DISABLE_AUDIO)
#define DT_AUDIO_DISABLED 1
#else
#define DT_AUDIO_DISABLED 0
#endif

#if !DT_AUDIO_DISABLED && defined(USE_PULSEAUDIO)
#define DT_AUDIO_USE_PULSEAUDIO 1
#else
#define DT_AUDIO_USE_PULSEAUDIO 0
#endif

#if !DT_AUDIO_DISABLED && defined(USE_ALSA)
#define DT_AUDIO_USE_ALSA 1
#else
#define DT_AUDIO_USE_ALSA 0
#endif

#if !DT_AUDIO_DISABLED && (defined(USE_OSS) || DT_PLATFORM_LINUX || defined(_SPARC_SOLARIS_))
#define DT_AUDIO_USE_OSS 1
#else
#define DT_AUDIO_USE_OSS 0
#endif

#if !DT_AUDIO_DISABLED && (defined(USE_AUDIOQUEUE) || DT_PLATFORM_APPLE)
#define DT_AUDIO_USE_AUDIOQUEUE 1
#else
#define DT_AUDIO_USE_AUDIOQUEUE 0
#endif

#if defined(_SPARC_SOLARIS_)
#define DT_AUDIO_LEGACY_OSS_DEVICE "/dev/audio"
#elif DT_PLATFORM_LINUX
#define DT_AUDIO_LEGACY_OSS_DEVICE "/dev/dsp"
#else
#define DT_AUDIO_LEGACY_OSS_DEVICE ""
#endif

void dt_audio_backend_get_compile_config(dt_audio_backend_config_t *config)
{
    if (config == 0) {
        return;
    }

    config->disable_audio = DT_AUDIO_DISABLED;
    config->use_oss = DT_AUDIO_USE_OSS;
    config->use_alsa = DT_AUDIO_USE_ALSA;
    config->use_pulseaudio = DT_AUDIO_USE_PULSEAUDIO;
    config->use_audioqueue = DT_AUDIO_USE_AUDIOQUEUE;
    config->legacy_oss_device = DT_AUDIO_LEGACY_OSS_DEVICE;
    config->alsa_default_device = "default";
    config->selection_order = "pulseaudio,alsa,oss,audioqueue";
}

const char *dt_audio_backend_name(dt_audio_backend_id_t backend)
{
    switch (backend) {
    case DT_AUDIO_BACKEND_NONE:
        return "none";
    case DT_AUDIO_BACKEND_OSS:
        return "oss";
    case DT_AUDIO_BACKEND_ALSA:
        return "alsa";
    case DT_AUDIO_BACKEND_PULSEAUDIO:
        return "pulseaudio";
    case DT_AUDIO_BACKEND_AUDIOQUEUE:
        return "audioqueue";
    default:
        return "unknown";
    }
}
