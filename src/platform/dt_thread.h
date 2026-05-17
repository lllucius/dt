/*
 * Purpose: internal thread wrapper declarations for platform abstraction.
 * Scope: private DECtalk modernization scaffolding; not a public API.
 * Behavior preservation: wrappers are standalone compile/smoke checks and do
 * not change existing thread lifecycle, queue, callback, or audio behavior.
 * Limitations: POSIX behavior is implemented first for the current Linux target.
 */

#ifndef DT_THREAD_H
#define DT_THREAD_H

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

typedef struct dt_thread dt_thread_t;
typedef void *(*dt_thread_proc_t)(void *context);

dt_thread_t *dt_thread_create(dt_thread_proc_t procedure, void *context);
int dt_thread_join(dt_thread_t *thread, void **status);
void dt_thread_destroy(dt_thread_t *thread);
void dt_thread_exit(void *status);

DT_EXTERN_C_END

#endif
