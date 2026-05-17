/*
 * Purpose: POSIX event wrapper implementation for internal scaffolding.
 * Scope: private Linux/POSIX synchronization checks.
 * Behavior preservation: preserves the existing manual-reset/auto-reset event
 * model for smoke tests without replacing current OP_* runtime events.
 * Limitations: timed waits use the default pthread condition-variable realtime
 * clock, matching the current POSIX condition-variable default.
 */

#include "dt_event.h"

#if DT_PLATFORM_POSIX

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

struct dt_event {
    int signaled;
    int manual_reset;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

static void dt_event_get_absolute_timeout(struct timespec *absolute_time,
    unsigned int timeout_milliseconds)
{
    clock_gettime(CLOCK_REALTIME, absolute_time);
    absolute_time->tv_sec += timeout_milliseconds / 1000U;
    absolute_time->tv_nsec += (long)(timeout_milliseconds % 1000U) * 1000000L;

    if (absolute_time->tv_nsec >= 1000000000L) {
        absolute_time->tv_sec++;
        absolute_time->tv_nsec -= 1000000000L;
    }
}

dt_event_t *dt_event_create(int manual_reset, int initial_state)
{
    dt_event_t *event = (dt_event_t *)malloc(sizeof(*event));

    if (event == NULL) {
        return NULL;
    }

    event->signaled = initial_state ? 1 : 0;
    event->manual_reset = manual_reset ? 1 : 0;

    if (pthread_mutex_init(&event->mutex, NULL) != 0) {
        free(event);
        return NULL;
    }

    if (pthread_cond_init(&event->condition, NULL) != 0) {
        pthread_mutex_destroy(&event->mutex);
        free(event);
        return NULL;
    }

    return event;
}

void dt_event_destroy(dt_event_t *event)
{
    if (event == NULL) {
        return;
    }

    pthread_cond_destroy(&event->condition);
    pthread_mutex_destroy(&event->mutex);
    free(event);
}

int dt_event_set(dt_event_t *event)
{
    int result;

    if (event == NULL) {
        return -1;
    }

    result = pthread_mutex_lock(&event->mutex);
    if (result != 0) {
        return result;
    }

    event->signaled = 1;
    result = pthread_cond_broadcast(&event->condition);
    pthread_mutex_unlock(&event->mutex);

    return result;
}

int dt_event_reset(dt_event_t *event)
{
    int result;

    if (event == NULL) {
        return -1;
    }

    result = pthread_mutex_lock(&event->mutex);
    if (result != 0) {
        return result;
    }

    event->signaled = 0;
    pthread_mutex_unlock(&event->mutex);

    return 0;
}

int dt_event_wait(dt_event_t *event, unsigned int timeout_milliseconds)
{
    int result;
    int wait_result = DT_EVENT_WAIT_NORMAL;
    struct timespec absolute_time;

    if (event == NULL) {
        return DT_EVENT_WAIT_ERROR;
    }

    result = pthread_mutex_lock(&event->mutex);
    if (result != 0) {
        return DT_EVENT_WAIT_ERROR;
    }

    if (timeout_milliseconds != DT_EVENT_WAIT_INFINITE) {
        dt_event_get_absolute_timeout(&absolute_time, timeout_milliseconds);
    }

    while (!event->signaled && wait_result == DT_EVENT_WAIT_NORMAL) {
        if (timeout_milliseconds == DT_EVENT_WAIT_INFINITE) {
            result = pthread_cond_wait(&event->condition, &event->mutex);
        } else {
            result = pthread_cond_timedwait(&event->condition, &event->mutex,
                &absolute_time);
        }

        if (result == ETIMEDOUT) {
            wait_result = DT_EVENT_WAIT_TIMEOUT;
        } else if (result != 0) {
            wait_result = DT_EVENT_WAIT_ERROR;
        }
    }

    if (wait_result == DT_EVENT_WAIT_NORMAL && !event->manual_reset) {
        event->signaled = 0;
    }

    pthread_mutex_unlock(&event->mutex);

    return wait_result;
}

#endif
