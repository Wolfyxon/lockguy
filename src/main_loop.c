#include "main_loop.h"

void main_loop(LoopInfo *info, AppState *state) {
    Component pass_inp = {
        .x = 0.5,
        .y = 0.5,
        .type = COMPONENT_PASSWORD_INPUT,
        .pass_inp = {},
    };

    Component clock = {
        .x = 0.5,
        .y = 0.4,
        .type = COMPONENT_CLOCK,
        .clock = {}
    };

    draw_clock(info, state, &clock);
    draw_password_input(info, state, &pass_inp);

    // TODO: Universal drawing functions

    if(state->allow_escape) {
        if(state->ctx_type == DISPLAY_CTX_X11) {
            x11_draw_text_centered(
                state,
                NULL, NULL, 
                info->offset_x + info->window_w * 0.5, 
                info->offset_y + info->window_h * 0.1, 
                "Press 'Esc' to exit lockscreen. Do not use this unless testing."
            );
        }
    }
}

void draw_clock(LoopInfo *info, AppState *state, Component *comp) {
    time_t now;
    
    time(&now);
    struct tm *tm_info = localtime(&now);

    char buf[20];
    strftime(buf, sizeof(buf) - 1, "%H:%M", tm_info);

    if(state->ctx_type == DISPLAY_CTX_X11) {
        XftFont *font = x11_load_font(state, "monospace", 64); // rather temp

        x11_draw_text_centered(
            state, 
            NULL, 
            font, 
            info->offset_x + comp->x * info->window_w, 
            info->offset_y + comp->y * info->window_h,  
            buf
        );

        x11_unload_font(state, font);
    }
}

void draw_password_input(LoopInfo *info, AppState *state, Component *comp) {
    if(state->ctx_type == DISPLAY_CTX_X11) {
        X11Context ctx = state->ctx.x11;
        
        float text_x = comp->x * info->window_w;
        float text_y = comp->y * info->window_h;
        
        float line_w_scale = strlen(state->password_buf) / 256.0;
        float line_y = text_y + 15;

        char *title = "Enter password";
        
        if(state->password_state == PASSWORD_CHECKING) {
            title = "Validating...";
        } else if(state->password_state == PASSWORD_INVALID) {
            title = "Invalid password";
        }

        x11_draw_text_centered(
            state, 
            NULL, NULL, 
            info->offset_x + text_x, info->offset_y + text_y, 
            title
        );

        XDrawLine(
            ctx.display, ctx.window, ctx.gc, 
            
            info->offset_x + (comp->x - line_w_scale) * info->window_w, 
            info->offset_y + line_y,
            
            info->offset_x + (comp->x + line_w_scale) * info->window_w,
            info->offset_y + line_y
        );
    }
}