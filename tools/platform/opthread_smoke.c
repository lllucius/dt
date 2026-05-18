/*
 * Purpose: smoke-test legacy OP_* threading primitives without routing DECtalk
 * runtime code through the newer src/platform wrappers.
 * Scope: developer verification tool built by the side-by-side CMake path for
 * the current Linux target.
 * Behavior preservation: records existing legacy thread, event, priority,
 * sleep, and lightweight-lock behavior as evidence before any wrapper adapter
 * or runtime wiring is considered.
 * Limitations: this is not a live-audio, callback, queue, pipe, timing, or
 * scheduler-conformance test. It checks deterministic legacy behavior only.
 */

#include "opthread.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

struct op_smoke_context {
    HEVENT_T started;
    HEVENT_T release;
    int value;
};

static void *op_smoke_thread_main(void *user_data)
{
    struct op_smoke_context *context = (struct op_smoke_context *)user_data;

    context->value = 77;
    OP_SetEvent(context->started);
    OP_WaitForEvent(context->release, OP_INFINITE);

    return (void *)(uintptr_t)77;
}

static int expect_wait_result(
    HEVENT_T event,
    unsigned int timeout_milliseconds,
    unsigned int expected_result,
    const char *label)
{
    unsigned int actual_result = OP_WaitForEvent(event, timeout_milliseconds);

    if (actual_result != expected_result) {
        fprintf(stderr, "%s: expected %u, got %u\n", label, expected_result,
            actual_result);
        return 1;
    }

    return 0;
}

static int run_event_smoke(void)
{
    HEVENT_T auto_event = OP_CreateEvent(FALSE, TRUE);
    HEVENT_T manual_event = OP_CreateEvent(TRUE, FALSE);
    int failed = 0;

    if (auto_event == NULL || manual_event == NULL) {
        fprintf(stderr, "opthread_smoke: failed to create events\n");
        failed = 1;
        goto cleanup;
    }

    failed |= expect_wait_result(
        auto_event,
        0,
        OP_WAIT_NORMAL,
        "auto-reset initial wait");
    failed |= expect_wait_result(
        auto_event,
        1,
        OP_WAIT_TIMEOUT,
        "auto-reset consumed wait");
    OP_SetEvent(auto_event);
    failed |= expect_wait_result(
        auto_event,
        0,
        OP_WAIT_NORMAL,
        "auto-reset set wait");
    failed |= expect_wait_result(
        auto_event,
        1,
        OP_WAIT_TIMEOUT,
        "auto-reset second consumed wait");

    failed |= expect_wait_result(
        manual_event,
        0,
        OP_WAIT_TIMEOUT,
        "manual-reset initial wait");
    OP_SetEvent(manual_event);
    failed |= expect_wait_result(
        manual_event,
        0,
        OP_WAIT_NORMAL,
        "manual-reset first set wait");
    failed |= expect_wait_result(
        manual_event,
        0,
        OP_WAIT_NORMAL,
        "manual-reset repeated wait");
    OP_ResetEvent(manual_event);
    failed |= expect_wait_result(
        manual_event,
        1,
        OP_WAIT_TIMEOUT,
        "manual-reset after reset");

cleanup:
    OP_DestroyEvent(manual_event);
    OP_DestroyEvent(auto_event);
    return failed;
}

static int run_mutex_smoke(void)
{
    HMUTEX_T mutex = OP_CreateMutex();

    if (mutex == NULL) {
        fprintf(stderr, "opthread_smoke: failed to create mutex\n");
        return 1;
    }

    OP_LockMutex(mutex);
    OP_UnlockMutex(mutex);
    OP_DestroyMutex(mutex);

    return 0;
}

static int run_lightweight_lock_smoke(void)
{
    TLOCK lock = PTHREAD_MUTEX_INITIALIZER;
    int first_lock;
    int second_lock;

    first_lock = ThreadLock(&lock, 1);
    second_lock = ThreadLock(&lock, 0);
    if (first_lock) {
        ThreadUnlock(&lock);
    }

    if (first_lock != 1 || second_lock != 0) {
        fprintf(stderr, "opthread_smoke: ThreadLock first=%d second=%d\n",
            first_lock, second_lock);
        return 1;
    }

    return 0;
}

static int run_thread_smoke(void)
{
    struct op_smoke_context context;
    HTHREAD_T thread;
    THREAD_STATUS_T thread_status = 0;
    THREAD_PRIORITY_T priority;
    unsigned int set_priority_status;
    unsigned int wait_status;
    int failed = 0;

    context.started = OP_CreateEvent(FALSE, FALSE);
    context.release = OP_CreateEvent(FALSE, FALSE);
    context.value = 0;
    if (context.started == NULL || context.release == NULL) {
        fprintf(stderr, "opthread_smoke: failed to create thread events\n");
        failed = 1;
        goto cleanup;
    }

    thread = OP_CreateThread(0, op_smoke_thread_main, &context);
    if (thread == NULL) {
        fprintf(stderr, "opthread_smoke: failed to create thread\n");
        failed = 1;
        goto cleanup;
    }

    failed |= expect_wait_result(
        context.started,
        1000,
        OP_WAIT_NORMAL,
        "thread started event");
    if (context.value != 77) {
        fprintf(stderr, "opthread_smoke: thread value=%d\n", context.value);
        failed = 1;
    }

    priority = OP_GetThreadPriority(thread);
    if (priority == OP_GET_THREAD_PRIORITY_ERROR) {
        fprintf(stderr, "opthread_smoke: OP_GetThreadPriority failed\n");
        failed = 1;
    }
    set_priority_status = OP_SetThreadPriority(thread, priority);
    if (set_priority_status != FALSE) {
        fprintf(stderr, "opthread_smoke: OP_SetThreadPriority returned %u\n",
            set_priority_status);
        failed = 1;
    }

    OP_Sleep(0);
    OP_SetEvent(context.release);
    wait_status = OP_WaitForThreadTermination(thread, &thread_status, 1000);
    if (wait_status != TRUE || (uintptr_t)thread_status != 77U) {
        fprintf(stderr, "opthread_smoke: wait=%u status=%" PRIuPTR "\n",
            wait_status, (uintptr_t)thread_status);
        failed = 1;
    }

    printf("op_thread_wait_status=%u\n", wait_status);
    printf("op_thread_return=%" PRIuPTR "\n", (uintptr_t)thread_status);
    printf("op_thread_priority=%d\n", priority);

cleanup:
    OP_DestroyEvent(context.release);
    OP_DestroyEvent(context.started);
    return failed;
}

int main(void)
{
    int failed = 0;

    failed |= run_mutex_smoke();
    failed |= run_event_smoke();
    failed |= run_lightweight_lock_smoke();
    failed |= run_thread_smoke();

    if (failed == 0) {
        printf("op_mutex=ok\n");
        printf("op_event_semantics=ok\n");
        printf("op_lightweight_lock=ok\n");
        printf("op_sleep_zero=ok\n");
        printf("opthread_smoke=ok\n");
    }

    return failed ? 1 : 0;
}
