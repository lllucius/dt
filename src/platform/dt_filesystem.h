/*
 * Purpose: internal filesystem helper declarations for platform wrappers.
 * Scope: private DECtalk modernization scaffolding; not a public API.
 * Behavior preservation: wrappers delegate to existing platform behavior and
 * do not change engine, dictionary, threading, or audio paths.
 * Limitations: POSIX behavior is implemented first because Linux is the
 * current supported target.
 */

#ifndef DT_FILESYSTEM_H
#define DT_FILESYSTEM_H

#include <stddef.h>

#include "dt_platform.h"

DT_EXTERN_C_BEGIN

int dt_path_is_absolute(const char *path);
int dt_path_join(char *buffer, size_t buffer_size, const char *base, const char *leaf);
int dt_path_exists(const char *path);

DT_EXTERN_C_END

#endif
