#include "dt_time.h"

#if DT_PLATFORM_POSIX

#include <errno.h>
#include <time.h>

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
