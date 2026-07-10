#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

#include <X11/Xlib.h>


#define PASSWORD_MAX_LEN_DEFAULT 1024 // if your password is that long please see a doctor

typedef enum {
    DISPLAY_CTX_X11,
    DISPLAY_CTX_WAYLAND
} DisplayContextType;

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
    char *password_buf;
    size_t password_max_len;
    bool mock;
    bool allow_escape;
} AppState;

void state_init_dynamic(AppState *state);
void state_init_password_buf(AppState *state);

void clear_password(AppState *state);