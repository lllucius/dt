/*
 * Purpose: internal mutex wrapper declarations for platform abstraction.
 * Scope: private DECtalk modernization scaffolding; not a public API.
 * Behavior preservation: wrappers are compiled and smoke-tested separately and
 * do not replace existing OP_* runtime synchronization paths.
 * Limitations: POSIX behavior is implemented first for the current Linux target.
 */

#ifndef DT_MUTEX_H
#define DT_MUTEX_H

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

typedef struct dt_mutex dt_mutex_t;

dt_mutex_t *dt_mutex_create(void);
void dt_mutex_destroy(dt_mutex_t *mutex);
int dt_mutex_lock(dt_mutex_t *mutex);
int dt_mutex_unlock(dt_mutex_t *mutex);

DT_EXTERN_C_END

#endif
