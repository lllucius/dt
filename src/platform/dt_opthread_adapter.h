/*
 * Purpose: private adapter declarations for legacy OP_* thread primitives.
 * Scope: CMake-only platform scaffolding and smoke tests for the current Linux
 * target.
 * Behavior preservation: the adapter delegates to src/dapi/src/nt/opthread.c
 * and preserves legacy handle ownership, wait return values, priority calls,
 * event semantics, sleep, and lightweight-lock behavior.
 * Limitations: this header is not installed, is not a public API, and does not
 * route DECtalk runtime code through src/platform.
 */

#ifndef DT_OPTHREAD_ADAPTER_H
#define DT_OPTHREAD_ADAPTER_H

#include "opthread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dt_opthread_adapter_thread {
    HTHREAD_T handle;
} DT_OPTHREAD_ADAPTER_THREAD;

typedef struct dt_opthread_adapter_mutex {
    HMUTEX_T handle;
} DT_OPTHREAD_ADAPTER_MUTEX;

typedef struct dt_opthread_adapter_event {
    HEVENT_T handle;
} DT_OPTHREAD_ADAPTER_EVENT;

void dt_opthread_adapter_thread_init(DT_OPTHREAD_ADAPTER_THREAD *thread);
int dt_opthread_adapter_thread_create(
    DT_OPTHREAD_ADAPTER_THREAD *thread,
    THREAD_STACK_SIZE_T stack_size,
    THREAD_PROCEDURE_T thread_routine,
    void *thread_data);
THREAD_PRIORITY_T dt_opthread_adapter_thread_get_priority(
    const DT_OPTHREAD_ADAPTER_THREAD *thread);
unsigned int dt_opthread_adapter_thread_set_priority(
    const DT_OPTHREAD_ADAPTER_THREAD *thread,
    THREAD_PRIORITY_T priority);
unsigned int dt_opthread_adapter_thread_wait(
    DT_OPTHREAD_ADAPTER_THREAD *thread,
    PTHREAD_STATUS_T thread_status,
    unsigned int timeout_milliseconds);

void dt_opthread_adapter_mutex_init(DT_OPTHREAD_ADAPTER_MUTEX *mutex);
int dt_opthread_adapter_mutex_create(DT_OPTHREAD_ADAPTER_MUTEX *mutex);
void dt_opthread_adapter_mutex_destroy(DT_OPTHREAD_ADAPTER_MUTEX *mutex);
void dt_opthread_adapter_mutex_lock(const DT_OPTHREAD_ADAPTER_MUTEX *mutex);
void dt_opthread_adapter_mutex_unlock(const DT_OPTHREAD_ADAPTER_MUTEX *mutex);

void dt_opthread_adapter_event_init(DT_OPTHREAD_ADAPTER_EVENT *event);
int dt_opthread_adapter_event_create(
    DT_OPTHREAD_ADAPTER_EVENT *event,
    unsigned int manual_reset,
    unsigned int initial_state);
void dt_opthread_adapter_event_destroy(DT_OPTHREAD_ADAPTER_EVENT *event);
void dt_opthread_adapter_event_set(const DT_OPTHREAD_ADAPTER_EVENT *event);
void dt_opthread_adapter_event_reset(const DT_OPTHREAD_ADAPTER_EVENT *event);
unsigned int dt_opthread_adapter_event_wait(
    const DT_OPTHREAD_ADAPTER_EVENT *event,
    unsigned int timeout_milliseconds);

void dt_opthread_adapter_sleep(unsigned int milliseconds);
int dt_opthread_adapter_light_lock(TLOCK *lock, unsigned int timeout_seconds);
void dt_opthread_adapter_light_unlock(TLOCK *lock);

#ifdef __cplusplus
}
#endif

#endif
