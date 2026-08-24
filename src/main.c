#include "main.h"

int main(int argc, char **argv) {
    #ifdef ENABLE_TESTS
    run_tests();
    #endif

    check_already_running();

    AppState state = {
        .ctx_type = DISPLAY_CTX_X11,
        .mock = false,
        .allow_escape = false,
        .screen_timeout_ms = 5000,
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

void check_already_running() {
    char path_self[PATH_MAX] = {0};
    ssize_t path_self_len = readlink("/proc/self/exe", path_self, sizeof(path_self));

    if(path_self_len == 0) {
        fprintf(stderr, "error: Failed to get path of executable. Not checking for another instance\n");
        return;
    }

    int pid = getpid();

    DIR *proc_dir = opendir("/proc");
    struct dirent *dir_entry;

    if(proc_dir == NULL) {
        fprintf(stderr, "error: Unable to access /proc\n");
        return;
    }

    while((dir_entry = readdir(proc_dir)) != NULL) {
        bool check_proc = true;
        
        for(size_t i = 0; i < strlen(dir_entry->d_name); i++) {
            // only check process dirs w pid to exclude all variants of self
            if(!isdigit(dir_entry->d_name[i])) {
                check_proc = false;
                break;
            }
        }

        if(!check_proc) {
            continue;
        }
        
        if(atoi(dir_entry->d_name) == pid) {
            continue;
        }

        char link_path[PATH_MAX] = {0};
        snprintf(link_path, sizeof(link_path), "/proc/%s/exe", dir_entry->d_name);

        char exe_path[PATH_MAX] = {0};
        ssize_t exe_len = readlink(link_path, exe_path, sizeof(exe_path));

        if(exe_len <= 0) {
            continue;
        }

        if(strcmp(path_self, exe_path) == 0) {
            fprintf(stderr, "error: Lockguy is already running\n");
            exit(1);
        }
    }
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
