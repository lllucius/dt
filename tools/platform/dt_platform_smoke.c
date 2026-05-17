/*
 * Purpose: compile and runtime smoke check for internal platform wrappers.
 * Scope: developer verification tool; it is not installed or used by DECtalk
 * runtime code.
 * Behavior preservation: exercises wrapper code without touching speech,
 * dictionary, threading, or audio behavior.
 * Limitations: intended for the current Linux/POSIX target and simple path/time
 * sanity checks only.
 */

#include "dt_filesystem.h"
#include "dt_time.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    const char *base = argc > 1 ? argv[1] : ".";
    const char *leaf = argc > 2 ? argv[2] : "PLAN.md";
    char joined_path[1024];
    uint64_t before_ms;
    uint64_t after_ms;

    if (dt_path_join(joined_path, sizeof(joined_path), base, leaf) != 0) {
        perror("dt_path_join");
        return 1;
    }

    if (dt_monotonic_milliseconds(&before_ms) != 0) {
        perror("dt_monotonic_milliseconds");
        return 1;
    }

    if (dt_sleep_milliseconds(0) != 0) {
        perror("dt_sleep_milliseconds");
        return 1;
    }

    if (dt_monotonic_milliseconds(&after_ms) != 0) {
        perror("dt_monotonic_milliseconds");
        return 1;
    }

    printf("path=%s\n", joined_path);
    printf("absolute=%d\n", dt_path_is_absolute(joined_path));
    printf("exists=%d\n", dt_path_exists(joined_path));
    printf("elapsed_ms=%" PRIu64 "\n", after_ms - before_ms);

    return dt_path_exists(joined_path) ? 0 : 2;
}
