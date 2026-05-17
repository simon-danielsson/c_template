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

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// semantics ------------------------------------------------------------------

typedef size_t usize;
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

#define LOG_POS_DETAILS __FILE_NAME__, __LINE__, __func__
#define LOG_PREFIX "=>"
#define LOG_SEP ":"

#if defined(NDEBUG)
#define ASSERT(cond, do_abort) ((void)0)
#define LOG(fmt, ...) ((void)0)
#else
#define ASSERT(cond, do_abort)                                                 \
    do {                                                                         \
        if ((cond)) {                                                              \
            fprintf(stderr, "%s %sSUCCESS%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,       \
                    LOG_STYLE, LOG_RESET, LOG_SEP, #cond, LOG_POS_DETAILS);          \
        } else {                                                                   \
            fprintf(stderr, "%s %sFAILURE%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,       \
                    LOG_STYLE, LOG_RESET, LOG_SEP, #cond, LOG_POS_DETAILS);          \
        }                                                                          \
        if (!(cond) && (do_abort)) {                                               \
            abort();                                                                 \
        }                                                                          \
    } while (0)

#define LOG(fmt, ...)                                                          \
    do {                                                                         \
        fprintf(stderr, "%s %sLOG%s %s %s [%s:%d (%s)]\n", LOG_PREFIX, LOG_STYLE,  \
                LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,                \
                LOG_POS_DETAILS);                                                  \
    } while (0)

#endif

// not implemented (todo msg that aborts the program)
#define NOT_IMPL(fmt, ...)                                                     \
    do {                                                                         \
        fprintf(stderr, "%s %sNOT IMPL%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,        \
                LOG_STYLE, LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,     \
                LOG_POS_DETAILS);                                                  \
        abort();                                                                   \
    } while (0)

#define PANIC(fmt, ...)                                                        \
    do {                                                                         \
        fprintf(stderr, "%s %sPANIC%s %s %s [%s:%d (%s)]\n", LOG_PREFIX,           \
                LOG_STYLE, LOG_RESET, LOG_SEP, fmt __VA_OPT__(, ) __VA_ARGS__,     \
                LOG_POS_DETAILS);                                                  \
        abort();                                                                   \
    } while (0)

#endif // MAIN_H
