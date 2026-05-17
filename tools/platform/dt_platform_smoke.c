/*
 * Purpose: compile and runtime smoke check for internal platform wrappers.
 * Scope: developer verification tool; it is not installed or used by DECtalk
 * runtime code.
 * Behavior preservation: exercises wrapper code without touching speech,
 * dictionary, threading, or audio behavior.
 * Limitations: intended for the current Linux/POSIX target and simple path/time
 * sanity checks only.
 */

#include "dt_filesystem.h"
#include "dt_event.h"
#include "dt_mutex.h"
#include "dt_thread.h"
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

    printf("path=%s\n", joined_path);
    printf("absolute=%d\n", dt_path_is_absolute(joined_path));
    printf("exists=%d\n", dt_path_exists(joined_path));
    printf("elapsed_ms=%" PRIu64 "\n", after_ms - before_ms);
    printf("thread_value=%d\n", smoke.value);

    dt_event_destroy(smoke.event);
    dt_mutex_destroy(smoke.mutex);

    return dt_path_exists(joined_path) ? 0 : 2;
}
