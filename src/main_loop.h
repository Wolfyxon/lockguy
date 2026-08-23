#pragma once

#include <time.h>

#include "context.h"
#include "x11.h"

typedef struct {
    float offset_x;
    float offset_y;

    float window_w;
    float window_h;
} LoopInfo;

void main_loop(LoopInfo *info, AppState *state);

void draw_clock(LoopInfo *info, AppState *state, Component *comp);
void draw_password_input(LoopInfo *info, AppState *state, Component *comp);
