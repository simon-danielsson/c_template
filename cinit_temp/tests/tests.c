#include "../src/main.h"

static void test1(void) { printf("test 1"); }
static void test2(void) { printf("test 2"); }
static void test3(void) { printf("test 3"); }

typedef void (*test_fn)(void);
static const test_fn tests[] = {
    [0] = NULL,
    [1] = test1,
    [2] = test2,
    [3] = test3,
};
void _run_test(int n) {
    size_t count = sizeof(tests) / sizeof(tests[0]);
    if (n > 0 && (size_t)n < count && tests[n]) {
        tests[n]();
        exit(0);
    }
}
