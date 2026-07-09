#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysymdef.h>

#include "context.h"

#define X11_ALL_POINTER_EVENTS ( \
    ButtonPressMask | ButtonReleaseMask |\
    EnterWindowMask | LeaveWindowMask | \
    PointerMotionMask | Button1MotionMask | Button2MotionMask |  \
    Button3MotionMask | Button4MotionMask | Button5MotionMask |  \
    ButtonMotionMask \
)

void x11_init(AppState *state);
void x11_loop(AppState *state);
