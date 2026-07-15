#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#include <X11/Xlib.h>

#include "component_defs.h"
#include "utils.h"

#define PASSWORD_MAX_LEN_DEFAULT 1024 // if your password is that long please see a doctor

typedef enum {
    DISPLAY_CTX_X11,
    DISPLAY_CTX_WAYLAND
} DisplayContextType;

typedef enum {
    SCREEN_SLEEP_DISABLED,
    SCREEN_SLEEP_BLACK,
    SCREEN_SLEEP_TURN_OFF
} ScreenSleepMode;

typedef struct {
    Display *display;
    Window window;
    Atom wm_delete_window;
    GC gc;
    XIC ic;
} X11Context;

typedef union {
    X11Context x11;
} DisplayContext;

typedef struct {
    DisplayContext ctx;
    DisplayContextType ctx_type;
    char *fallback_command;
    char *password_buf;
    size_t password_max_len;
    bool mock;
    bool allow_escape;
    Component *components;
    size_t components_len;
    millis_t last_input_time;
    bool screen_off;
    millis_t screen_timeout_ms;
    ScreenSleepMode screen_sleep_mode;
} AppState;

void state_init_dynamic(AppState *state);
void state_init_password_buf(AppState *state);
void state_init_component_buf(AppState *state);

void clear_password(AppState *state);