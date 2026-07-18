#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "tests.h"
#include "utils.h"
#include "auth.h"
#include "context.h"
#include "fallback.h"
#include "x11_runtime.h"

#ifndef VERSION
#define VERSION "unknown"
#endif

int main(int argc, char **argv);
void process_args(AppState *ctx, int argc, char **argv);
void process_warnings(AppState *state);

void print_help();
