/*
 * Purpose: source-level inventory and opt-in gate for historical targets.
 * Scope: private DECtalk modernization scaffolding; not a public API.
 * Behavior preservation: current Linux builds pass through unchanged, while
 * non-current target macros require explicit source opt-in in this scaffold.
 * Limitations: this header documents and gates platform scaffolding only; it
 * does not delete, rewrite, or prove historical target behavior.
 */

#ifndef DT_LEGACY_TARGETS_H
#define DT_LEGACY_TARGETS_H

#if !defined(DECTALK_ENABLE_LEGACY_TARGET_SOURCE)
#if defined(WIN32) || defined(_WIN32) || defined(UNDER_CE) || \
    defined(_WIN32_WCE_EMULATION) || defined(__osf__) || \
    defined(_SPARC_SOLARIS_) || defined(VXWORKS) || defined(MSDOS) || \
    defined(ARM7) || defined(EPSON_ARM7) || defined(__ipaq__) || \
    defined(__ppc__) || defined(_APPLE_MAC_) || defined(__APPLE__) || \
    defined(__EMSCRIPTEN__) || defined(ALPHA)
#error "Historical DECtalk target macro active without DECTALK_ENABLE_LEGACY_TARGET_SOURCE"
#endif
#endif

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

typedef struct dt_legacy_target_inventory {
    int source_opt_in;
    int windows;
    int windows_ce;
    int osf_tru64;
    int sparc_solaris;
    int vxworks;
    int msdos;
    int arm7;
    int ipaq_linux;
    int powerpc_mac;
    int apple;
    int emscripten;
    int alpha;
} dt_legacy_target_inventory_t;

void dt_legacy_targets_get_inventory(dt_legacy_target_inventory_t *inventory);
const char *dt_legacy_targets_source_opt_in_macro(void);

DT_EXTERN_C_END

#endif
