#pragma once

#ifdef ENABLE_TESTS

#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

#define exec_test(func) do { \
    printf("-- Running '%s' -- \n", #func); \
    func(); \
} while(0);

void run_tests();

#endif