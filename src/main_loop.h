#pragma once

#include "context.h"

typedef struct {
    float window_w;
    float window_h;
} LoopInfo;

void main_loop(LoopInfo *info, AppState *state);
void draw_password_input(LoopInfo *info, AppState *state, Component *comp);
