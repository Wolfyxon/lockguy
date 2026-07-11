#include "x11.h"

void x11_init(AppState *state) {
    Display *disp = XOpenDisplay(NULL);

    if(disp == NULL) {
        fprintf(stderr, "error: Unable to open display\n");
        exit(1);
    }

    int screen = DefaultScreen(disp);    

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

    XSetWindowBackground(disp, w, 0x000000);
    XMapWindow(disp, w);
    XRaiseWindow(disp, w);
    
    if(!state->mock) {
        x11_finalize_guardian_window(disp, w);
    }

    state->ctx.x11.gc = DefaultGC(disp, screen);

    state->ctx.x11.display = disp;
    state->ctx.x11.window = w;
    state->ctx.x11.wm_delete_window = wm_delete_window;
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
            XWindowAttributes attr = {0};
            XGetWindowAttributes(ctx.display, ctx.window, &attr);
            
            LoopInfo info = {
                .window_w = attr.width,
                .window_h = attr.height
            };

            XClearWindow(ctx.display, ctx.window);
            XSetForeground(ctx.display, ctx.gc, 0xFFFFFF);

            main_loop(&info, state);

            usleep(1600);
        }

        if(!state->mock) {
            XRaiseWindow(ctx.display, ctx.window);
        }

        while(XPending(ctx.display)) {
            x11_handle_next_event(state);
        }
    }
}

void x11_interacted(AppState *state) {
    state->last_input_time = get_time_ms();

    // TODO: Wake up screen
}

void x11_handle_next_event(AppState *state) {
    X11Context ctx = state->ctx.x11;
    XEvent ev;

    XNextEvent(ctx.display, &ev);
    
    if(ev.type == MotionNotify) {
        x11_interacted(state);
    }

    if(ev.type == KeyPress) {
        x11_interacted(state);

        KeySym ks = XLookupKeysym(&ev.xkey, 0);
        
        if(ks == XK_Escape && (state->mock || state->allow_escape)) {
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

        if(len != 0) {
            strcat_safe(state->password_buf, input_buf, state->password_max_len);
        }
    }

    if(ev.type == ClientMessage) {
        if(ev.xclient.data.l[0] == ctx.wm_delete_window) {
            exit(0);
        }
    }
}
