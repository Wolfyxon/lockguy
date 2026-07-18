#include "main_loop.h"

void main_loop(LoopInfo *info, AppState *state) {
    Component pass_inp = {
        .x = 0.5,
        .y = 0.5,
        .type = COMPONENT_PASSWORD_INPUT,
        .pass_inp = {},
    };

    draw_password_input(info, state, &pass_inp);
}

void init_password_input(AppState *state, Component *comp) {
    comp->type = COMPONENT_PASSWORD_INPUT;
    
}

XftFont *font = NULL;
XftColor color;
bool color_ye = false;

void draw_password_input(LoopInfo *info, AppState *state, Component *comp) {
    if(state->ctx_type == DISPLAY_CTX_X11) {
        X11Context ctx = state->ctx.x11;
        
        float text_x = comp->x * info->window_w;
        float text_y = comp->y * info->window_h;
        
        float line_w_scale = strlen(state->password_buf) / 1024.0;
        float line_y = text_y + 10;

        x11_draw_text(state, NULL, NULL, text_x, text_y, "hello there");

        XDrawLine(
            ctx.display, ctx.window, ctx.gc, 
            
            (comp->x - line_w_scale) * info->window_w, 
            line_y, 
            
            (comp->x + line_w_scale) * info->window_w,
            line_y
        );
    }
}