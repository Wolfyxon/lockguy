#include "tests.h"

#ifdef ENABLE_TESTS

void test_strcat_safe() {
    char str[128] = "hello ";
    strcat_safe(str, "world", sizeof(str));

    if(strcmp(str, "hello world") != 0) {
        fprintf(stderr, "error: '%s' != 'hello world'", str);
        exit(1);
    }
}

void run_tests() {
    exec_test(test_strcat_safe);

    puts("All tests successful");
    exit(0);
}

#endif