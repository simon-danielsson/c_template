#include "../src/main.h"

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

void test1() { printf("test 1"); }
void test2() { printf("test 2"); }
void test3() { printf("test 3"); }
