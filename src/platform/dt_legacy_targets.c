/*
 * Purpose: compile-time historical target inventory for quarantine checks.
 * Scope: private platform scaffolding used by developer smoke tests.
 * Behavior preservation: reports macro state only and does not route DECtalk
 * runtime, build, audio, dictionary, or public API behavior.
 * Limitations: historical target presence is inferred from preprocessor macros;
 * this file does not validate those targets or claim support for them.
 */

#include "dt_legacy_targets.h"

#if defined(DECTALK_ENABLE_LEGACY_TARGET_SOURCE)
#define DT_LEGACY_SOURCE_OPT_IN 1
#else
#define DT_LEGACY_SOURCE_OPT_IN 0
#endif

#if defined(WIN32) || defined(_WIN32)
#define DT_LEGACY_WINDOWS 1
#else
#define DT_LEGACY_WINDOWS 0
#endif

#if defined(UNDER_CE) || defined(_WIN32_WCE_EMULATION)
#define DT_LEGACY_WINDOWS_CE 1
#else
#define DT_LEGACY_WINDOWS_CE 0
#endif

#if defined(__osf__)
#define DT_LEGACY_OSF_TRU64 1
#else
#define DT_LEGACY_OSF_TRU64 0
#endif

#if defined(_SPARC_SOLARIS_) || defined(__sparc)
#define DT_LEGACY_SPARC_SOLARIS 1
#else
#define DT_LEGACY_SPARC_SOLARIS 0
#endif

#if defined(VXWORKS)
#define DT_LEGACY_VXWORKS 1
#else
#define DT_LEGACY_VXWORKS 0
#endif

#if defined(MSDOS)
#define DT_LEGACY_MSDOS 1
#else
#define DT_LEGACY_MSDOS 0
#endif

#if defined(ARM7) || defined(EPSON_ARM7)
#define DT_LEGACY_ARM7 1
#else
#define DT_LEGACY_ARM7 0
#endif

#if defined(__ipaq__)
#define DT_LEGACY_IPAQ_LINUX 1
#else
#define DT_LEGACY_IPAQ_LINUX 0
#endif

#if defined(__ppc__) || defined(_APPLE_MAC_)
#define DT_LEGACY_POWERPC_MAC 1
#else
#define DT_LEGACY_POWERPC_MAC 0
#endif

#if defined(__APPLE__)
#define DT_LEGACY_APPLE 1
#else
#define DT_LEGACY_APPLE 0
#endif

#if defined(__EMSCRIPTEN__)
#define DT_LEGACY_EMSCRIPTEN 1
#else
#define DT_LEGACY_EMSCRIPTEN 0
#endif

#if defined(ALPHA)
#define DT_LEGACY_ALPHA 1
#else
#define DT_LEGACY_ALPHA 0
#endif

void dt_legacy_targets_get_inventory(dt_legacy_target_inventory_t *inventory)
{
    if (inventory == 0) {
        return;
    }

    inventory->source_opt_in = DT_LEGACY_SOURCE_OPT_IN;
    inventory->windows = DT_LEGACY_WINDOWS;
    inventory->windows_ce = DT_LEGACY_WINDOWS_CE;
    inventory->osf_tru64 = DT_LEGACY_OSF_TRU64;
    inventory->sparc_solaris = DT_LEGACY_SPARC_SOLARIS;
    inventory->vxworks = DT_LEGACY_VXWORKS;
    inventory->msdos = DT_LEGACY_MSDOS;
    inventory->arm7 = DT_LEGACY_ARM7;
    inventory->ipaq_linux = DT_LEGACY_IPAQ_LINUX;
    inventory->powerpc_mac = DT_LEGACY_POWERPC_MAC;
    inventory->apple = DT_LEGACY_APPLE;
    inventory->emscripten = DT_LEGACY_EMSCRIPTEN;
    inventory->alpha = DT_LEGACY_ALPHA;
}

const char *dt_legacy_targets_source_opt_in_macro(void)
{
    return "DECTALK_ENABLE_LEGACY_TARGET_SOURCE";
}
