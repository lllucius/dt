/*
 * Purpose: POSIX filesystem helper implementations for internal wrappers.
 * Scope: private Linux/POSIX scaffolding used by modernization tools first.
 * Behavior preservation: functions wrap simple libc/POSIX behavior without
 * changing DECtalk runtime, dictionary, threading, or audio paths.
 * Limitations: path joining uses POSIX '/' separators and does not canonicalize
 * paths, resolve symlinks, or allocate memory.
 */

#include "dt_filesystem.h"

#if DT_PLATFORM_POSIX

#include <errno.h>
#include <string.h>
#include <unistd.h>

int dt_path_is_absolute(const char *path)
{
    return path != NULL && path[0] == '/';
}

int dt_path_join(char *buffer, size_t buffer_size, const char *base, const char *leaf)
{
    size_t base_len;
    size_t leaf_len;
    int needs_separator;

    if (buffer == NULL || buffer_size == 0 || leaf == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (dt_path_is_absolute(leaf) || base == NULL || base[0] == '\0') {
        leaf_len = strlen(leaf);
        if (leaf_len + 1 > buffer_size) {
            errno = ENAMETOOLONG;
            return -1;
        }
        memcpy(buffer, leaf, leaf_len + 1);
        return 0;
    }

    base_len = strlen(base);
    leaf_len = strlen(leaf);
    needs_separator = base_len > 0 && base[base_len - 1] != '/';

    if (base_len + (size_t)needs_separator + leaf_len + 1 > buffer_size) {
        errno = ENAMETOOLONG;
        return -1;
    }

    memcpy(buffer, base, base_len);
    if (needs_separator) {
        buffer[base_len] = '/';
        base_len++;
    }
    memcpy(buffer + base_len, leaf, leaf_len + 1);

    return 0;
}

int dt_path_exists(const char *path)
{
    if (path == NULL || path[0] == '\0') {
        return 0;
    }

    return access(path, F_OK) == 0;
}

#endif
