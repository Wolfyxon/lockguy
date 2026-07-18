#pragma once

#define ENABLE_TESTS

#ifdef ENABLE_TESTS

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

#include "utils.h"
#include "context.h"
#include "x11.h"

#define exec_test(func) do { \
    printf("-- Running '%s' -- \n", #func); \
    func(); \
} while(0);

void run_tests();

#endif