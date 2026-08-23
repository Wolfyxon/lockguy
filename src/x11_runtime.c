#include "x11_runtime.h"

void x11_init_minimal(AppState *state) {\
    Display *disp = XOpenDisplay(NULL);

    if(disp == NULL) {
        fprintf(stderr, "error: Unable to open display\n");
        exit(1);
    }

    int screen = DefaultScreen(disp);

    state->ctx_type = DISPLAY_CTX_X11;
    state->ctx.x11.screen = screen;
    state->ctx.x11.visual = DefaultVisual(disp, screen);
    state->ctx.x11.colormap = DefaultColormap(disp, screen);
    state->ctx.x11.display = disp;
}

void x11_init_xft(AppState *state) {
    X11Context ctx = state->ctx.x11;

    XftDraw *xft_draw = XftDrawCreate(ctx.display, ctx.window, state->ctx.x11.visual, state->ctx.x11.colormap);
    
    if(xft_draw == NULL) {
        fprintf(stderr, "error: Unable to create XftDraw\n");
        exit(1);
    }

    state->ctx.x11.xft_draw = xft_draw;
    
    state->ctx.x11.default_color = x11_load_color(state, DEFAULT_COLOR);
    state->ctx.x11.default_font = x11_load_font(state, DEFAULT_FONT, DEFAULT_FONT_SIZE);
}

void x11_init(AppState *state) {
    x11_init_minimal(state);

    Display *disp = state->ctx.x11.display;

    int screen = state->ctx.x11.screen;

    int width = DisplayWidth(disp, screen);
    int height = DisplayHeight(disp, screen);

    unsigned long vmask = CWOverrideRedirect; // makes the window top level & allows key grab

    XSetWindowAttributes attr = {
        .override_redirect = !state->mock // makes the window top level but 2
    };

    Window w = XCreateWindow(
        disp, DefaultRootWindow(disp), 
        0, 0, // x, y 
        width, height, 
        0,  // border width
        DefaultDepth(disp, screen), 
        InputOutput, // class
        DefaultVisual(disp, screen),
        vmask, &attr
    );
    
    Atom wm_delete_window = XInternAtom(disp, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(disp, w, &wm_delete_window, 1);
    XSelectInput(disp, w, KeyPressMask | PointerMotionMask);

    GC gc = DefaultGC(disp, screen);
    
    XSetWindowBackground(disp, w, 0x000000);
    XMapWindow(disp, w);
    XRaiseWindow(disp, w);
    
    if(!state->mock) {
        x11_finalize_guardian_window(disp, w);
    }

    state->ctx.x11.gc = gc;
    state->ctx.x11.window = w;
    state->ctx.x11.wm_delete_window = wm_delete_window;

    x11_init_xft(state);
}

void x11_cleanup(AppState *state) {
    X11Context ctx = state->ctx.x11;

    if(ctx.gc != NULL) {
        XFreeGC(ctx.display, ctx.gc);
    }

    if(ctx.xft_draw != NULL) {
        XftDrawDestroy(ctx.xft_draw);
    }

    XCloseDisplay(ctx.display);
    
    ctx.display = NULL;
}

void x11_finalize_guardian_window(Display *disp, Window w) {
    int grab_kb = XGrabKeyboard(disp, w, true, GrabModeAsync, GrabModeAsync, CurrentTime);
    int grab_mouse = XGrabPointer(disp, w, true, X11_ALL_POINTER_EVENTS, GrabModeAsync, GrabModeAsync, w, None, CurrentTime);

    if(grab_kb != GrabSuccess) {
        fprintf(stderr, "error: Unable to grab keyboard input. Code: %d\n", grab_kb);
        exit(1);
    }

    if(grab_mouse != GrabSuccess) {
        fprintf(stderr, "error: Unable to grab mouse pointer. Code: %d\n", grab_mouse);
        exit(1);
    }

    // Makes the window always on top (also hides notifications)
    XWindowChanges changes;
    changes.stack_mode = Above;

    XConfigureWindow(disp, w, CWStackMode, &changes);
}

void x11_screen_sleep(AppState *state) {
    state->screen_off = true;
    
    if(state->screen_sleep_mode == SCREEN_SLEEP_BLACK) {
        XClearWindow(state->ctx.x11.display, state->ctx.x11.window);
        return;
    }
    
    if(state->screen_sleep_mode == SCREEN_SLEEP_TURN_OFF && !state->mock) {
        DPMSForceLevel(state->ctx.x11.display, DPMSModeOff);
    }
}

void x11_screen_wakeup(AppState *state) {
    if(!state->screen_off) return;

    state->screen_off = false;

    if(state->screen_sleep_mode == SCREEN_SLEEP_TURN_OFF && !state->mock) {
        DPMSForceLevel(state->ctx.x11.display, DPMSModeOn);
    }
}

void x11_run_loop(AppState *state) {
    X11Context ctx = state->ctx.x11;

    // Values here don't seem to matter. They only delay the start of the X11 connection. 
    // No matter how high the values are, events are stil received instantly 
    // and the render loop runs continously without any lag
    struct timeval event_timeout_timer = {
        .tv_sec = 0,
        .tv_usec = 5000
    };

    int x11_socket = ConnectionNumber(ctx.display);
    fd_set events_fd;

    while(1) {
        FD_ZERO(&events_fd);
        FD_SET(x11_socket, &events_fd);

        int received_events = select(
            x11_socket + 1, 
            &events_fd, 
            NULL, 
            NULL, 
            &event_timeout_timer
        );

        if(received_events == 0) { // Timeout. No pending events.
            if(!state->screen_off) {
                LoopInfo info = x11_get_loop_info(state);

                XClearWindow(ctx.display, ctx.window);
                XSetForeground(ctx.display, ctx.gc, 0xFFFFFF);

                main_loop(&info, state);
                x11_check_sleep(state);
            }

            usleep(8000);
        }

        if(!state->mock) {
            XRaiseWindow(ctx.display, ctx.window);
        }

        while(XPending(ctx.display)) {
            x11_handle_next_event(state);
        }
    }
}

LoopInfo x11_get_loop_info(AppState *state) {
    X11Context ctx = state->ctx.x11;
    
    XWindowAttributes attr = {0};
    XGetWindowAttributes(ctx.display, ctx.window, &attr);
    
    LoopInfo info = {
        .window_w = attr.width,
        .window_h = attr.height
    };

    if(!state->mock) {
        int monitors_len = 0;
        XRRMonitorInfo *monitors = XRRGetMonitors(ctx.display, ctx.window, True, &monitors_len);
        
        if(monitors == NULL) {
            return info;
        }

        for(size_t i = 0; i < monitors_len; i++) {
            XRRMonitorInfo m = monitors[i];
            
            if(m.primary) {
                info.offset_x = m.x;
                info.offset_y = m.y;
                info.window_w = m.width;
                info.window_h = m.height;
                
                break;
            }
        }

        XRRFreeMonitors(monitors);
    }

    return info;
}

void x11_check_sleep(AppState *state) {
    if(state->mock) {
        return;
    }

    if(state->screen_sleep_mode == SCREEN_SLEEP_DISABLED) {
        return;
    }
    
    if(state->last_input_time + state->screen_timeout_ms < get_time_ms()) {
        x11_screen_sleep(state);
    }
}

void x11_interacted(AppState *state) {
    state->last_input_time = get_time_ms();
    x11_screen_wakeup(state);
}

void x11_handle_next_event(AppState *state) {
    X11Context ctx = state->ctx.x11;
    XEvent ev;

    XNextEvent(ctx.display, &ev);
    
    bool screen_was_off = state->screen_off;

    if(ev.type == MotionNotify) {
        x11_interacted(state);
    }

    if(ev.type == KeyPress) {
        x11_interacted(state);

        KeySym ks = XLookupKeysym(&ev.xkey, 0);
        
        if(ks == XK_Escape && (state->mock || state->allow_escape)) {
            if(screen_was_off) {
                usleep(250000); // wait for screen to wake up
                                         // If the lockscreen exits when the screen is still off, systemd triggers a configured lockscreen
            }
            
            exit(0);
        }

        if(ks == XK_Return) {
            AuthStatus status = auth_current_user(state->password_buf);
            clear_password(state);

            if(status == AUTH_SUCCESS) {                
                exit(0);
            }

            return;
        }

        if(ks == XK_BackSpace) {
            strcut_back(state->password_buf, 1);
            return;
        }
        
        // Text input for password

        char input_buf[8] = {0};
        size_t len = XLookupString(&ev.xkey, input_buf, sizeof(input_buf) - 1, NULL, NULL);

        if(len != 0 && input_buf[0] != '\x1b') {
            strcat_safe(state->password_buf, input_buf, state->password_max_len);
        }
    }

    if(ev.type == ClientMessage) {
        if(ev.xclient.data.l[0] == ctx.wm_delete_window) {
            exit(0);
        }
    }
}
