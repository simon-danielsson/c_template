#include "main.h"

typedef void (*test_fn)(void);
static const test_fn tests[] = {
    [0] = NULL,
    [1] = test1,
    [2] = test2,
    [3] = test3,
};
static void run_test(int n) {
    size_t count = sizeof(tests) / sizeof(tests[0]);
    if (n > 0 && (size_t)n < count && tests[n]) {
        tests[n]();
        exit(0);
    }
}

int main(int argc, char **argv) {
    run_test(TEST);
    printf("%s\n", ENV_NAME);
    return 0;
}
