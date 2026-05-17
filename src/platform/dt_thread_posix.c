/*
 * Purpose: POSIX thread wrapper implementation for internal scaffolding.
 * Scope: private Linux/POSIX platform abstraction checks.
 * Behavior preservation: delegates directly to pthread create/join/exit and is
 * not wired into DECtalk runtime thread lifecycle code.
 * Limitations: created thread handles must be joined or otherwise externally
 * completed before destruction.
 */

#include "dt_thread.h"

#if DT_PLATFORM_POSIX

#include <pthread.h>
#include <stdlib.h>

struct dt_thread {
    pthread_t native_thread;
    int joined;
};

dt_thread_t *dt_thread_create(dt_thread_proc_t procedure, void *context)
{
    dt_thread_t *thread;

    if (procedure == NULL) {
        return NULL;
    }

    thread = (dt_thread_t *)malloc(sizeof(*thread));
    if (thread == NULL) {
        return NULL;
    }

    thread->joined = 0;
    if (pthread_create(&thread->native_thread, NULL, procedure, context) != 0) {
        free(thread);
        return NULL;
    }

    return thread;
}

int dt_thread_join(dt_thread_t *thread, void **status)
{
    int result;

    if (thread == NULL || thread->joined) {
        return -1;
    }

    result = pthread_join(thread->native_thread, status);
    if (result == 0) {
        thread->joined = 1;
    }

    return result;
}

void dt_thread_destroy(dt_thread_t *thread)
{
    free(thread);
}

void dt_thread_exit(void *status)
{
    pthread_exit(status);
}

#endif
