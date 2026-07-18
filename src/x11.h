#pragma once

#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "context.h"

#define DEFAULT_COLOR "#ffffff"
#define DEFAULT_FONT "mono"
#define DEFAULT_FONT_SIZE 12

XftFont *x11_load_font(AppState *state, const char *name, double size); // never returns NULL

bool x11_try_load_color(AppState *state, const char *name, XftColor *res);
XftColor x11_load_color(AppState *state, const char *name);
void x11_free_color(AppState *state, XftColor *color);

void x11_draw_text(AppState *state, XftColor *color, XftFont *font, float x, float y, const char *text);
