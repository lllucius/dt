/*
 * Purpose: compile and runtime smoke check for internal platform wrappers.
 * Scope: developer verification tool; it is not installed or used by DECtalk
 * runtime code.
 * Behavior preservation: exercises wrapper code without touching speech,
 * dictionary, threading, or audio behavior.
 * Limitations: intended for the current Linux/POSIX target and simple path/time
 * sanity checks only.
 */

#include "dt_audio_backend.h"
#include "dt_filesystem.h"
#include "dt_event.h"
#include "dt_mutex.h"
#include "dt_thread.h"
#include "dt_legacy_targets.h"
#include "dt_time.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

struct smoke_context {
    dt_mutex_t *mutex;
    dt_event_t *event;
    int value;
};

static void *smoke_thread_main(void *context)
{
    struct smoke_context *smoke = (struct smoke_context *)context;

    if (dt_mutex_lock(smoke->mutex) != 0) {
        return (void *)(uintptr_t)1;
    }
    smoke->value = 42;
    if (dt_mutex_unlock(smoke->mutex) != 0) {
        return (void *)(uintptr_t)2;
    }
    if (dt_event_set(smoke->event) != 0) {
        return (void *)(uintptr_t)3;
    }

    return 0;
}

static int expect_event_wait(dt_event_t *event, unsigned int timeout_milliseconds,
    int expected_result, const char *label)
{
    int actual_result = dt_event_wait(event, timeout_milliseconds);

    if (actual_result != expected_result) {
        fprintf(stderr, "%s: expected wait result %d, got %d\n", label,
            expected_result, actual_result);
        return 1;
    }

    return 0;
}

static int run_event_semantics_smoke(void)
{
    dt_event_t *auto_event = dt_event_create(0, 1);
    dt_event_t *manual_event = dt_event_create(1, 0);
    int status = 0;

    if (auto_event == NULL || manual_event == NULL) {
        fprintf(stderr, "failed to create event semantic wrappers\n");
        status = 1;
        goto cleanup;
    }

    if (expect_event_wait(auto_event, 0, DT_EVENT_WAIT_NORMAL,
            "auto-reset initial wait") != 0 ||
        expect_event_wait(auto_event, 1, DT_EVENT_WAIT_TIMEOUT,
            "auto-reset consumed wait") != 0 ||
        dt_event_set(auto_event) != 0 ||
        expect_event_wait(auto_event, 0, DT_EVENT_WAIT_NORMAL,
            "auto-reset set wait") != 0 ||
        expect_event_wait(auto_event, 1, DT_EVENT_WAIT_TIMEOUT,
            "auto-reset second consumed wait") != 0) {
        status = 1;
        goto cleanup;
    }

    if (expect_event_wait(manual_event, 0, DT_EVENT_WAIT_TIMEOUT,
            "manual-reset unsignaled wait") != 0 ||
        dt_event_set(manual_event) != 0 ||
        expect_event_wait(manual_event, 0, DT_EVENT_WAIT_NORMAL,
            "manual-reset first signaled wait") != 0 ||
        expect_event_wait(manual_event, 0, DT_EVENT_WAIT_NORMAL,
            "manual-reset repeated signaled wait") != 0 ||
        dt_event_reset(manual_event) != 0 ||
        expect_event_wait(manual_event, 1, DT_EVENT_WAIT_TIMEOUT,
            "manual-reset reset wait") != 0) {
        status = 1;
        goto cleanup;
    }

cleanup:
    dt_event_destroy(manual_event);
    dt_event_destroy(auto_event);
    return status;
}

int main(int argc, char **argv)
{
    const char *base = argc > 1 ? argv[1] : ".";
    const char *leaf = argc > 2 ? argv[2] : "PLAN.md";
    char joined_path[1024];
    uint64_t before_ms;
    uint64_t after_ms;
    dt_thread_t *thread;
    void *thread_status = NULL;
    struct smoke_context smoke;
    dt_audio_backend_config_t audio_config;
    dt_legacy_target_inventory_t legacy_inventory;

    if (dt_path_join(joined_path, sizeof(joined_path), base, leaf) != 0) {
        perror("dt_path_join");
        return 1;
    }

    if (dt_monotonic_milliseconds(&before_ms) != 0) {
        perror("dt_monotonic_milliseconds");
        return 1;
    }

    if (dt_sleep_milliseconds(0) != 0) {
        perror("dt_sleep_milliseconds");
        return 1;
    }

    if (dt_monotonic_milliseconds(&after_ms) != 0) {
        perror("dt_monotonic_milliseconds");
        return 1;
    }

    smoke.mutex = dt_mutex_create();
    smoke.event = dt_event_create(0, 0);
    smoke.value = 0;
    if (smoke.mutex == NULL || smoke.event == NULL) {
        fprintf(stderr, "failed to create synchronization wrappers\n");
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    thread = dt_thread_create(smoke_thread_main, &smoke);
    if (thread == NULL) {
        fprintf(stderr, "failed to create thread wrapper\n");
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    if (dt_event_wait(smoke.event, 1000) != DT_EVENT_WAIT_NORMAL) {
        fprintf(stderr, "event wait did not complete normally\n");
        dt_thread_join(thread, &thread_status);
        dt_thread_destroy(thread);
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    if (dt_thread_join(thread, &thread_status) != 0 ||
        (uintptr_t)thread_status != 0) {
        fprintf(stderr, "thread join failed\n");
        dt_thread_destroy(thread);
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    dt_thread_destroy(thread);

    if (dt_mutex_lock(smoke.mutex) != 0) {
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }
    if (smoke.value != 42) {
        dt_mutex_unlock(smoke.mutex);
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }
    dt_mutex_unlock(smoke.mutex);

    if (dt_event_wait(smoke.event, 1) != DT_EVENT_WAIT_TIMEOUT) {
        fprintf(stderr, "auto-reset thread event remained signaled\n");
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    if (run_event_semantics_smoke() != 0) {
        dt_event_destroy(smoke.event);
        dt_mutex_destroy(smoke.mutex);
        return 1;
    }

    printf("path=%s\n", joined_path);
    printf("absolute=%d\n", dt_path_is_absolute(joined_path));
    printf("exists=%d\n", dt_path_exists(joined_path));
    printf("elapsed_ms=%" PRIu64 "\n", after_ms - before_ms);
    printf("thread_value=%d\n", smoke.value);
    printf("event_semantics=ok\n");
    dt_audio_backend_get_compile_config(&audio_config);
    printf("audio_disabled=%d\n", audio_config.disable_audio);
    printf("audio_oss=%d\n", audio_config.use_oss);
    printf("audio_alsa=%d\n", audio_config.use_alsa);
    printf("audio_pulseaudio=%d\n", audio_config.use_pulseaudio);
    printf("audio_audioqueue=%d\n", audio_config.use_audioqueue);
    printf("audio_legacy_oss_device=%s\n", audio_config.legacy_oss_device);
    printf("audio_selection_order=%s\n", audio_config.selection_order);
    dt_legacy_targets_get_inventory(&legacy_inventory);
    printf("legacy_source_opt_in=%d\n", legacy_inventory.source_opt_in);
    printf("legacy_windows=%d\n", legacy_inventory.windows);
    printf("legacy_windows_ce=%d\n", legacy_inventory.windows_ce);
    printf("legacy_osf_tru64=%d\n", legacy_inventory.osf_tru64);
    printf("legacy_sparc_solaris=%d\n", legacy_inventory.sparc_solaris);
    printf("legacy_vxworks=%d\n", legacy_inventory.vxworks);
    printf("legacy_msdos=%d\n", legacy_inventory.msdos);
    printf("legacy_arm7=%d\n", legacy_inventory.arm7);
    printf("legacy_ipaq_linux=%d\n", legacy_inventory.ipaq_linux);
    printf("legacy_powerpc_mac=%d\n", legacy_inventory.powerpc_mac);
    printf("legacy_apple=%d\n", legacy_inventory.apple);
    printf("legacy_emscripten=%d\n", legacy_inventory.emscripten);
    printf("legacy_alpha=%d\n", legacy_inventory.alpha);

    dt_event_destroy(smoke.event);
    dt_mutex_destroy(smoke.mutex);

    return dt_path_exists(joined_path) ? 0 : 2;
}
