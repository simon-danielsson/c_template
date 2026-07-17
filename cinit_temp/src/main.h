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

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#endif // MAIN_H
