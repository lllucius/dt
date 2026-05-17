#include "dt_time.h"

#if DT_PLATFORM_POSIX

#include <errno.h>
#include <stdint.h>
#include <time.h>

int dt_monotonic_milliseconds(uint64_t *milliseconds)
{
    struct timespec current_time;

    if (milliseconds == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &current_time) != 0) {
        return -1;
    }

    *milliseconds = ((uint64_t)current_time.tv_sec * 1000U) +
        ((uint64_t)current_time.tv_nsec / 1000000U);

    return 0;
}

int dt_sleep_milliseconds(unsigned int milliseconds)
{
    struct timespec requested;

    requested.tv_sec = milliseconds / 1000U;
    requested.tv_nsec = (long)(milliseconds % 1000U) * 1000000L;

    while (nanosleep(&requested, &requested) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }

    return 0;
}

#endif
