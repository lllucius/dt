/*
 * Purpose: POSIX mutex wrapper implementation for internal scaffolding.
 * Scope: private Linux/POSIX platform abstraction checks.
 * Behavior preservation: delegates directly to pthread mutex behavior and is
 * not wired into DECtalk runtime synchronization paths.
 * Limitations: this wrapper owns heap-allocated mutex handles only.
 */

#include "dt_mutex.h"

#if DT_PLATFORM_POSIX

#include <pthread.h>
#include <stdlib.h>

struct dt_mutex {
    pthread_mutex_t native_mutex;
};

dt_mutex_t *dt_mutex_create(void)
{
    dt_mutex_t *mutex = (dt_mutex_t *)malloc(sizeof(*mutex));

    if (mutex == NULL) {
        return NULL;
    }

    if (pthread_mutex_init(&mutex->native_mutex, NULL) != 0) {
        free(mutex);
        return NULL;
    }

    return mutex;
}

void dt_mutex_destroy(dt_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    pthread_mutex_destroy(&mutex->native_mutex);
    free(mutex);
}

int dt_mutex_lock(dt_mutex_t *mutex)
{
    if (mutex == NULL) {
        return -1;
    }

    return pthread_mutex_lock(&mutex->native_mutex);
}

int dt_mutex_unlock(dt_mutex_t *mutex)
{
    if (mutex == NULL) {
        return -1;
    }

    return pthread_mutex_unlock(&mutex->native_mutex);
}

#endif
