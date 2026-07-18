#include "x11.h"

XftFont *x11_load_font(AppState *state, const char *name, double size) {
    X11Context ctx = state->ctx.x11;

    if(name == NULL) {
        return NULL;
    }

    return XftFontOpen(ctx.display, ctx.screen, XFT_FAMILY, XftTypeString, name, XFT_SIZE, XftTypeDouble, size, NULL);
}

bool x11_try_load_color(AppState *state, const char *name, XftColor *res) {
    X11Context ctx = state->ctx.x11;
    return XftColorAllocName(ctx.display, ctx.visual, ctx.colormap, name, res);
}

XftColor x11_load_color(AppState *state, const char *name) {
    XftColor color = {0};
    bool res = x11_try_load_color(state, name, &color);

    if(res) {
        return color;
    }

    bool fallback_res = x11_try_load_color(state, DEFAULT_COLOR, &color);

    if(!fallback_res) {
        fprintf(stderr, "error: Failed to load fallback color '%s' after '%s' failed\n", DEFAULT_COLOR, name);
        exit(1);
    }

    return color;
}

void x11_free_color(AppState *state, XftColor *color) {
    XftColorFree(state->ctx.x11.display, state->ctx.x11.visual, state->ctx.x11.colormap, color);
}

void x11_draw_text(AppState *state, XftColor *color, XftFont *font, float x, float y, const char *text) {
    X11Context ctx = state->ctx.x11;
    
    XftDrawStringUtf8(
        ctx.xft_draw, 
        color != NULL ? color : &ctx.default_color, 
        font != NULL ? font : ctx.default_font, 
        x, y, 
        (const FcChar8 *)text, 
        strlen(text)
    );
}
