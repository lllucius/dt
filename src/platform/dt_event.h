/*
 * Purpose: internal event wrapper declarations for synchronization scaffolding.
 * Scope: private DECtalk modernization work; not a public API.
 * Behavior preservation: wrappers mimic the existing manual-reset/auto-reset
 * event shape but are not wired into OP_* runtime event paths.
 * Limitations: POSIX behavior is implemented first for the current Linux target.
 */

#ifndef DT_EVENT_H
#define DT_EVENT_H

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

#define DT_EVENT_WAIT_INFINITE ((unsigned int)(-1))
#define DT_EVENT_WAIT_NORMAL 0
#define DT_EVENT_WAIT_TIMEOUT 1
#define DT_EVENT_WAIT_ERROR 2

typedef struct dt_event dt_event_t;

dt_event_t *dt_event_create(int manual_reset, int initial_state);
void dt_event_destroy(dt_event_t *event);
int dt_event_set(dt_event_t *event);
int dt_event_reset(dt_event_t *event);
int dt_event_wait(dt_event_t *event, unsigned int timeout_milliseconds);

DT_EXTERN_C_END

#endif
