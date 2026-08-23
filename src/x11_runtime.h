#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <X11/extensions/dpms.h>
#include <X11/Xft/Xft.h>

#include "context.h"
#include "utils.h"
#include "auth.h"
#include "main_loop.h"

#define X11_ALL_POINTER_EVENTS ( \
    ButtonPressMask | ButtonReleaseMask |\
    EnterWindowMask | LeaveWindowMask | \
    PointerMotionMask | Button1MotionMask | Button2MotionMask |  \
    Button3MotionMask | Button4MotionMask | Button5MotionMask |  \
    ButtonMotionMask \
)

void x11_init_minimal(AppState *state); // for tests
void x11_init(AppState *state);
void x11_init_xft(AppState *state);
void x11_cleanup(AppState *state);
void x11_finalize_guardian_window(Display *disp, Window w);

void x11_interacted(AppState *state);
void x11_screen_sleep(AppState *state);
void x11_screen_wakeup(AppState *state);

void x11_interacted(AppState *state);
void x11_check_sleep(AppState *state);

void x11_run_loop(AppState *state);
LoopInfo x11_get_loop_info(AppState *state);
void x11_handle_next_event(AppState *state);
