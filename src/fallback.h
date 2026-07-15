#pragma once

#include <sys/wait.h>

#include "context.h"

#define FALLBACK_FLAG "fallback"

void fallback_mode_run(AppState *state, int argc, char **argv);
void fallback_fill_argv(char **dest, int old_argc, char **old_argv, int new_argc);
size_t get_argc_for_child(int argc, char **argv);
int fallback_run_self(int argc, char **argv);

