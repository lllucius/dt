/*
 * Purpose: smoke-test the private OP_* adapter without routing DECtalk runtime
 * code through src/platform.
 * Scope: developer verification tool built by the side-by-side CMake path for
 * the current Linux target.
 * Behavior preservation: compares adapter-visible thread, event, mutex,
 * priority, sleep, and lightweight-lock behavior against known legacy OP_*
 * semantics while preserving legacy return values.
 * Limitations: this is not a live-audio, callback, queue, pipe, timing, or
 * scheduler-conformance test.
 */

#include "dt_opthread_adapter.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

struct adapter_smoke_context {
    DT_OPTHREAD_ADAPTER_EVENT started;
    DT_OPTHREAD_ADAPTER_EVENT release;
    int value;
};

static void *adapter_smoke_thread_main(void *user_data)
{
    struct adapter_smoke_context *context =
        (struct adapter_smoke_context *)user_data;

    context->value = 91;
    dt_opthread_adapter_event_set(&context->started);
    dt_opthread_adapter_event_wait(&context->release, OP_INFINITE);

    return (void *)(uintptr_t)91;
}

static int expect_wait_result(
    const DT_OPTHREAD_ADAPTER_EVENT *event,
    unsigned int timeout_milliseconds,
    unsigned int expected_result,
    const char *label)
{
    unsigned int actual_result =
        dt_opthread_adapter_event_wait(event, timeout_milliseconds);

    if (actual_result != expected_result) {
        fprintf(stderr, "%s: expected %u, got %u\n", label, expected_result,
            actual_result);
        return 1;
    }

    return 0;
}

static int run_event_smoke(void)
{
    DT_OPTHREAD_ADAPTER_EVENT auto_event;
    DT_OPTHREAD_ADAPTER_EVENT manual_event;
    int failed = 0;

    dt_opthread_adapter_event_init(&auto_event);
    dt_opthread_adapter_event_init(&manual_event);
    if (!dt_opthread_adapter_event_create(&auto_event, FALSE, TRUE) ||
        !dt_opthread_adapter_event_create(&manual_event, TRUE, FALSE)) {
        fprintf(stderr, "dt_opthread_adapter_smoke: failed to create events\n");
        failed = 1;
        goto cleanup;
    }

    failed |= expect_wait_result(
        &auto_event,
        0,
        OP_WAIT_NORMAL,
        "adapter auto-reset initial wait");
    failed |= expect_wait_result(
        &auto_event,
        1,
        OP_WAIT_TIMEOUT,
        "adapter auto-reset consumed wait");
    dt_opthread_adapter_event_set(&auto_event);
    failed |= expect_wait_result(
        &auto_event,
        0,
        OP_WAIT_NORMAL,
        "adapter auto-reset set wait");
    failed |= expect_wait_result(
        &auto_event,
        1,
        OP_WAIT_TIMEOUT,
        "adapter auto-reset second consumed wait");

    failed |= expect_wait_result(
        &manual_event,
        0,
        OP_WAIT_TIMEOUT,
        "adapter manual-reset initial wait");
    dt_opthread_adapter_event_set(&manual_event);
    failed |= expect_wait_result(
        &manual_event,
        0,
        OP_WAIT_NORMAL,
        "adapter manual-reset first set wait");
    failed |= expect_wait_result(
        &manual_event,
        0,
        OP_WAIT_NORMAL,
        "adapter manual-reset repeated wait");
    dt_opthread_adapter_event_reset(&manual_event);
    failed |= expect_wait_result(
        &manual_event,
        1,
        OP_WAIT_TIMEOUT,
        "adapter manual-reset after reset");

cleanup:
    dt_opthread_adapter_event_destroy(&manual_event);
    dt_opthread_adapter_event_destroy(&auto_event);
    return failed;
}

static int run_mutex_smoke(void)
{
    DT_OPTHREAD_ADAPTER_MUTEX mutex;

    dt_opthread_adapter_mutex_init(&mutex);
    if (!dt_opthread_adapter_mutex_create(&mutex)) {
        fprintf(stderr, "dt_opthread_adapter_smoke: failed to create mutex\n");
        return 1;
    }

    dt_opthread_adapter_mutex_lock(&mutex);
    dt_opthread_adapter_mutex_unlock(&mutex);
    dt_opthread_adapter_mutex_destroy(&mutex);

    return 0;
}

static int run_lightweight_lock_smoke(void)
{
    TLOCK lock = PTHREAD_MUTEX_INITIALIZER;
    int first_lock;
    int second_lock;

    first_lock = dt_opthread_adapter_light_lock(&lock, 1);
    second_lock = dt_opthread_adapter_light_lock(&lock, 0);
    if (first_lock) {
        dt_opthread_adapter_light_unlock(&lock);
    }

    if (first_lock != 1 || second_lock != 0) {
        fprintf(
            stderr,
            "dt_opthread_adapter_smoke: ThreadLock first=%d second=%d\n",
            first_lock,
            second_lock);
        return 1;
    }

    return 0;
}

static int run_thread_smoke(void)
{
    struct adapter_smoke_context context;
    DT_OPTHREAD_ADAPTER_THREAD thread;
    THREAD_STATUS_T thread_status = 0;
    THREAD_PRIORITY_T priority;
    unsigned int set_priority_status;
    unsigned int wait_status;
    int failed = 0;

    dt_opthread_adapter_event_init(&context.started);
    dt_opthread_adapter_event_init(&context.release);
    context.value = 0;
    if (!dt_opthread_adapter_event_create(&context.started, FALSE, FALSE) ||
        !dt_opthread_adapter_event_create(&context.release, FALSE, FALSE)) {
        fprintf(
            stderr,
            "dt_opthread_adapter_smoke: failed to create thread events\n");
        failed = 1;
        goto cleanup;
    }

    dt_opthread_adapter_thread_init(&thread);
    if (!dt_opthread_adapter_thread_create(
            &thread,
            64U * 1024U,
            adapter_smoke_thread_main,
            &context)) {
        fprintf(stderr, "dt_opthread_adapter_smoke: failed to create thread\n");
        failed = 1;
        goto cleanup;
    }

    failed |= expect_wait_result(
        &context.started,
        1000,
        OP_WAIT_NORMAL,
        "adapter thread started event");
    if (context.value != 91) {
        fprintf(
            stderr,
            "dt_opthread_adapter_smoke: thread value=%d\n",
            context.value);
        failed = 1;
    }

    priority = dt_opthread_adapter_thread_get_priority(&thread);
    if (priority == OP_GET_THREAD_PRIORITY_ERROR) {
        fprintf(stderr, "dt_opthread_adapter_smoke: get priority failed\n");
        failed = 1;
    }
    set_priority_status =
        dt_opthread_adapter_thread_set_priority(&thread, priority);
    if (set_priority_status != FALSE) {
        fprintf(
            stderr,
            "dt_opthread_adapter_smoke: set priority returned %u\n",
            set_priority_status);
        failed = 1;
    }

    dt_opthread_adapter_sleep(0);
    dt_opthread_adapter_event_set(&context.release);
    wait_status =
        dt_opthread_adapter_thread_wait(&thread, &thread_status, 1000);
    if (wait_status != TRUE || (uintptr_t)thread_status != 91U) {
        fprintf(
            stderr,
            "dt_opthread_adapter_smoke: wait=%u status=%" PRIuPTR "\n",
            wait_status,
            (uintptr_t)thread_status);
        failed = 1;
    }

    printf("adapter_thread_stack_size=65536\n");
    printf("adapter_thread_wait_status=%u\n", wait_status);
    printf("adapter_thread_return=%" PRIuPTR "\n", (uintptr_t)thread_status);
    printf("adapter_thread_priority=%d\n", priority);

cleanup:
    dt_opthread_adapter_event_destroy(&context.release);
    dt_opthread_adapter_event_destroy(&context.started);
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
        printf("adapter_mutex=ok\n");
        printf("adapter_event_semantics=ok\n");
        printf("adapter_lightweight_lock=ok\n");
        printf("adapter_sleep_zero=ok\n");
        printf("dt_opthread_adapter_smoke=ok\n");
    }

    return failed ? 1 : 0;
}
