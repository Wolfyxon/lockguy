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
    
    XSetWindowBackground(disp, w, 0x000000);
    XMapWindow(disp, w);
    
    if(!state->mock) {
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

    Atom wm_delete_window = XInternAtom(disp, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(disp, w, &wm_delete_window, 1);
    XSelectInput(disp, w, ExposureMask | KeyPressMask);

    state->ctx.x11.display = disp;
    state->ctx.x11.window = w;
    state->ctx.x11.wm_delete_window = wm_delete_window;
}

void x11_loop(AppState *state) {
    X11Context ctx = state->ctx.x11;

    XEvent ev;

    while(1) {
        XNextEvent(ctx.display, &ev);
        XRaiseWindow(ctx.display, ctx.window);
        
        if(ev.type == Expose) {
            // render stuff here
            
        }

        if(ev.type == KeyPress) {
            if(ev.xkey.keycode == 9 && (state->mock || state->allow_escape)) { // esc. Who knows where keycodes are defined
                exit(0);
            }
        }

        if(ev.type == ClientMessage) {
            if(ev.xclient.data.l[0] == ctx.wm_delete_window) {
                exit(0);
            }
        }
    }
}
