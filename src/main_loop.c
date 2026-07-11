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

void draw_password_input(LoopInfo *info, AppState *state, Component *comp) {
    if(state->ctx_type == DISPLAY_CTX_X11) {
        X11Context ctx = state->ctx.x11;
        
        float text_x = comp->x * info->window_w;
        float text_y = comp->y * info->window_h;

        // TODO: Center the text
        XTextItem text = {
            .chars = "Enter password",
            .nchars = 14,
            .delta = 0,
        };
                
        float line_w_scale = strlen(state->password_buf) / 1024.0;
        float line_y = text_y + 10;

        XDrawText(ctx.display, ctx.window, ctx.gc, text_x, text_y, &text, 1);
        
        XDrawLine(
            ctx.display, ctx.window, ctx.gc, 
            
            (comp->x - line_w_scale) * info->window_w, 
            line_y, 
            
            (comp->x + line_w_scale) * info->window_w,
            line_y
        );
    }
}