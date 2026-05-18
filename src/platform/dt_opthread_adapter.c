/*
 * Purpose: private adapter implementation for legacy OP_* thread primitives.
 * Scope: CMake-only platform scaffolding and smoke tests for the current Linux
 * target.
 * Behavior preservation: this file delegates to src/dapi/src/nt/opthread.c
 * without normalizing legacy return values, handle ownership, priority
 * behavior, event semantics, sleep behavior, or lightweight-lock behavior.
 * Limitations: this adapter is not linked into DECtalk runtime libraries and
 * is not a replacement for opthread.c.
 */

#include "dt_opthread_adapter.h"

#include <stddef.h>

void dt_opthread_adapter_thread_init(DT_OPTHREAD_ADAPTER_THREAD *thread)
{
    if (thread != NULL) {
        thread->handle = NULL;
    }
}

int dt_opthread_adapter_thread_create(
    DT_OPTHREAD_ADAPTER_THREAD *thread,
    THREAD_STACK_SIZE_T stack_size,
    THREAD_PROCEDURE_T thread_routine,
    void *thread_data)
{
    if (thread == NULL) {
        return FALSE;
    }

    thread->handle = OP_CreateThread(stack_size, thread_routine, thread_data);
    return thread->handle != NULL;
}

THREAD_PRIORITY_T dt_opthread_adapter_thread_get_priority(
    const DT_OPTHREAD_ADAPTER_THREAD *thread)
{
    if (thread == NULL) {
        return OP_GET_THREAD_PRIORITY_ERROR;
    }

    return OP_GetThreadPriority(thread->handle);
}

unsigned int dt_opthread_adapter_thread_set_priority(
    const DT_OPTHREAD_ADAPTER_THREAD *thread,
    THREAD_PRIORITY_T priority)
{
    if (thread == NULL) {
        return TRUE;
    }

    return OP_SetThreadPriority(thread->handle, priority);
}

unsigned int dt_opthread_adapter_thread_wait(
    DT_OPTHREAD_ADAPTER_THREAD *thread,
    PTHREAD_STATUS_T thread_status,
    unsigned int timeout_milliseconds)
{
    unsigned int wait_status;

    if (thread == NULL || thread->handle == NULL) {
        return TRUE;
    }

#if defined WIN32 && !defined NOWIN
    (void)timeout_milliseconds;
    wait_status = OP_WaitForThreadTermination(thread->handle, thread_status);
#else
    wait_status = OP_WaitForThreadTermination(
        thread->handle,
        thread_status,
        timeout_milliseconds);
#endif
    thread->handle = NULL;
    return wait_status;
}

void dt_opthread_adapter_mutex_init(DT_OPTHREAD_ADAPTER_MUTEX *mutex)
{
    if (mutex != NULL) {
        mutex->handle = NULL;
    }
}

int dt_opthread_adapter_mutex_create(DT_OPTHREAD_ADAPTER_MUTEX *mutex)
{
    if (mutex == NULL) {
        return FALSE;
    }

    mutex->handle = OP_CreateMutex();
    return mutex->handle != NULL;
}

void dt_opthread_adapter_mutex_destroy(DT_OPTHREAD_ADAPTER_MUTEX *mutex)
{
    if (mutex != NULL) {
        OP_DestroyMutex(mutex->handle);
        mutex->handle = NULL;
    }
}

void dt_opthread_adapter_mutex_lock(const DT_OPTHREAD_ADAPTER_MUTEX *mutex)
{
    if (mutex != NULL) {
        OP_LockMutex(mutex->handle);
    }
}

void dt_opthread_adapter_mutex_unlock(const DT_OPTHREAD_ADAPTER_MUTEX *mutex)
{
    if (mutex != NULL) {
        OP_UnlockMutex(mutex->handle);
    }
}

void dt_opthread_adapter_event_init(DT_OPTHREAD_ADAPTER_EVENT *event)
{
    if (event != NULL) {
        event->handle = NULL;
    }
}

int dt_opthread_adapter_event_create(
    DT_OPTHREAD_ADAPTER_EVENT *event,
    unsigned int manual_reset,
    unsigned int initial_state)
{
    if (event == NULL) {
        return FALSE;
    }

    event->handle = OP_CreateEvent(manual_reset, initial_state);
    return event->handle != NULL;
}

void dt_opthread_adapter_event_destroy(DT_OPTHREAD_ADAPTER_EVENT *event)
{
    if (event != NULL) {
        OP_DestroyEvent(event->handle);
        event->handle = NULL;
    }
}

void dt_opthread_adapter_event_set(const DT_OPTHREAD_ADAPTER_EVENT *event)
{
    if (event != NULL) {
        OP_SetEvent(event->handle);
    }
}

void dt_opthread_adapter_event_reset(const DT_OPTHREAD_ADAPTER_EVENT *event)
{
    if (event != NULL) {
        OP_ResetEvent(event->handle);
    }
}

unsigned int dt_opthread_adapter_event_wait(
    const DT_OPTHREAD_ADAPTER_EVENT *event,
    unsigned int timeout_milliseconds)
{
    if (event == NULL) {
        return OP_WAIT_ABANDONED;
    }

    return OP_WaitForEvent(event->handle, timeout_milliseconds);
}

void dt_opthread_adapter_sleep(unsigned int milliseconds)
{
    OP_Sleep(milliseconds);
}

int dt_opthread_adapter_light_lock(TLOCK *lock, unsigned int timeout_seconds)
{
    return ThreadLock(lock, timeout_seconds);
}

void dt_opthread_adapter_light_unlock(TLOCK *lock)
{
    ThreadUnlock(lock);
}
