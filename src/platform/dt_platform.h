#ifndef DT_PLATFORM_H
#define DT_PLATFORM_H

#if defined(__linux__)
#define DT_PLATFORM_LINUX 1
#else
#define DT_PLATFORM_LINUX 0
#endif

#if defined(_WIN32)
#define DT_PLATFORM_WINDOWS 1
#else
#define DT_PLATFORM_WINDOWS 0
#endif

#if defined(__APPLE__)
#define DT_PLATFORM_APPLE 1
#else
#define DT_PLATFORM_APPLE 0
#endif

#if DT_PLATFORM_LINUX || DT_PLATFORM_APPLE
#define DT_PLATFORM_POSIX 1
#else
#define DT_PLATFORM_POSIX 0
#endif

#if defined(__cplusplus)
#define DT_EXTERN_C_BEGIN extern "C" {
#define DT_EXTERN_C_END }
#else
#define DT_EXTERN_C_BEGIN
#define DT_EXTERN_C_END
#endif

#endif
