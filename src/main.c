#include "main.h"

int main(int argc, char **argv) {
    #ifdef ENABLE_TESTS
    run_tests();
    #endif

    AppState state = {
        .ctx_type = DISPLAY_CTX_X11,
        .mock = false,
        .allow_escape = true,
        .screen_timeout_ms = 2000,
        .screen_sleep_mode = SCREEN_SLEEP_TURN_OFF
    };
    
    state_init_dynamic(&state);

    process_args(&state, argc, argv);

    if(state.fallback_command) {
        fallback_mode_run(&state, argc, argv);
        return 0;
    }

    process_warnings(&state);

    x11_init(&state);
    x11_run_loop(&state);
}

void process_warnings(AppState *state) {
    if(state->allow_escape && !state->mock) {
        fprintf(stderr, "warning: Lock screen is bypassable using Escape! Do not use unless testing\n");
    }

    if(state->allow_escape && state->mock) {
        fprintf(stderr, "warning: --mock already enables --allow-esc. Remove --allow-esc.\n");
    }
}

void process_args(AppState *state, int argc, char **argv) {
    for(size_t i = 1; i < argc; i++) {
        char *arg = argv[i];

        if(is_flag(arg, "help")) {
            print_help();
            exit(0);
        } 
        else if(is_flag(arg, "version")) {
            printf("lockguy v. %s\n", VERSION);  
            exit(0);
        }
        else if(is_flag(arg, "mock")) {
            state->mock = true;
        }
        else if(is_flag(arg, "allow-esc")) {
            state->allow_escape = true;
        }
        else if(is_flag(arg, "fallback")) {
            if(argc < i + 2) {
                fprintf(stderr, "error: --fallback requires a value\n");
                exit(1);
            }

            state->fallback_command = argv[i + 1];
            i++;
        }
        else {
            fprintf(stderr, "error: Unrecognized argument '%s'\n", arg);
            exit(1);
        }
    }
}

void print_help() {
    puts("Usage: lockguy [option]...");
    puts("");
    puts("Options:");
    puts(" --help:               Show this help");
    puts(" --version:            Show program version");
    puts(" --mock:               Run in a normal window instead of locking the screen");
    puts(" --allow-esc:          Allow exit using escape key. DO NOT USE UNLESS TESTING!");
    puts(" --fallback <command>: Runs a command if the lockscreen crashes (can be another lockscreen)");
    puts("");
    puts("Bugs, help & source code at:");
    puts("https://github.com/Wolfyxon/lockguy");
}
