#ifndef MAIN_H
#define MAIN_H

#if defined(NDEBUG)
#define BUILD_RELEASE 1
#define BUILD_DEBUG 0
#else
#define BUILD_RELEASE 0
#define BUILD_DEBUG 1
#endif

#if defined(TEST)
#define BUILD_TEST 1
#else
#define BUILD_TEST 0
#endif

// standard libraries ---------------------------------------------------------

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// semantics ------------------------------------------------------------------

typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;

// suppress warnings ----------------------------------------------------------

#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

// project variables ----------------------------------------------------------

#ifndef ENV_NAME // project name
#define ENV_NAME "UNDEFINED"
#endif
#ifndef ENV_AUTHOR // project author
#define ENV_AUTHOR "UNDEFINED"
#endif
#ifndef ENV_CONTACT // author contact
#define ENV_CONTACT "UNDEFINED"
#endif
#ifndef ENV_GITHASH // git version hash
#define ENV_GITHASH "UNDEFINED"
#endif
#ifndef ENV_GITTAG // git release tag
#define ENV_GITTAG "UNDEFINED"
#endif
#ifndef ENV_REPO // git repo
#define ENV_REPO "UNDEFINED"
#endif

// logging --------------------------------------------------------------------

#define LOG_STYLE "\x1b[1m"
#define LOG_RESET "\x1b[0m"

#ifndef __FILE_NAME__
#define __FILE_NAME__ __FILE__
#endif

static inline void _log_template(const char *file, size_t file_len, i32 line,
        const char *func, size_t func_len, char *type,
        const char *fmt, ...) {
    char msg[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(msg, sizeof(msg), fmt, args);
    va_end(args);

    char file_name[24];
    if (file_len > 11) {
        snprintf(file_name, sizeof(file_name), "%.10s…", file);
    } else {
        strcpy(file_name, file);
    }

    char fn_name[24];
    if (func_len > 11) {
        snprintf(fn_name, sizeof(fn_name), "%.10s…", func);
    } else {
        strcpy(fn_name, func);
    }

    printf("\033[1m%-8s\033[0m ┆ %-11s ┆ %-4d ┆ %-11s ┆ %s\n", type, file_name,
            line, fn_name, msg);
}

#if defined(NDEBUG)
#define ASSERT(cond, do_abort) ((void)0)
#define LOG(fmt, ...) ((void)0)
#else
#define ASSERT(cond, do_abort)                                                 \
    do {                                                                         \
        if ((cond)) {                                                              \
            _log_template(__FILE_NAME__, strlen(__FILE_NAME__), __LINE__, __func__,  \
                    strlen(__func__), "SUCCESS", "%s", #cond);                 \
        } else {                                                                   \
            _log_template(__FILE_NAME__, strlen(__FILE_NAME__), __LINE__, __func__,  \
                    strlen(__func__), "FAILURE", "%s", #cond);                 \
        }                                                                          \
        if (!(cond) && (do_abort)) {                                               \
            abort();                                                                 \
        }                                                                          \
    } while (0)

#define LOG(fmt, ...)                                                          \
    do {                                                                         \
        _log_template(__FILE_NAME__, strlen(__FILE_NAME__), __LINE__, __func__,    \
                strlen(__func__), "LOG", fmt __VA_OPT__(, ) __VA_ARGS__);    \
    } while (0)

#endif

// not implemented (todo msg that aborts the program)
#define NOT_IMPL(fmt, ...)                                                     \
    do {                                                                         \
        _log_template(__FILE_NAME__, strlen(__FILE_NAME__), __LINE__, __func__,    \
                strlen(__func__), "NOT IMPL",                                \
                fmt __VA_OPT__(, ) __VA_ARGS__);                             \
        abort();                                                                   \
    } while (0)

#define PANIC(fmt, ...)                                                        \
    do {                                                                         \
        _log_template(__FILE_NAME__, strlen(__FILE_NAME__), __LINE__, __func__,    \
                strlen(__func__), "PANIC", fmt __VA_OPT__(, ) __VA_ARGS__);  \
        abort();                                                                   \
    } while (0)

#endif // MAIN_H
