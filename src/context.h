#pragma once

#include <X11/Xlib.h>

typedef enum {
    DISPLAY_CTX_X11,
    DISPLAY_CTX_WAYLAND
} DisplayContextType;

typedef struct {
    Display *display;
    Window window;
    Atom wm_delete_window;
} X11Context;

typedef union {
    X11Context x11;
} DisplayContext;

typedef struct {
    DisplayContext ctx;
    DisplayContextType ctx_type;
    bool mock;
    bool allow_escape;
} AppState;
