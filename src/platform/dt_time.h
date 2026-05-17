#ifndef DT_TIME_H
#define DT_TIME_H

#include <stdint.h>

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

int dt_monotonic_milliseconds(uint64_t *milliseconds);
int dt_sleep_milliseconds(unsigned int milliseconds);

DT_EXTERN_C_END

#endif
