#include "fallback.h"

void fallback_mode_run(AppState *state, int argc, char **argv) {
    if(strbegins(state->fallback_command, "lockguy")) {
        fprintf(stderr, "warning: Possibly using self as fallback. You may end up in a crash loop!\n");
    }

    int child_status = fallback_run_self(argc, argv);

    if(child_status == 0) {
        exit(0);
    }

    fprintf(stderr, "error: Lockscreen died with code %d. Running fallback command.\n", child_status);

    int fallback_status = system(state->fallback_command);
    exit(fallback_status);
}

size_t get_argc_for_child(int argc, char **argv) {
    size_t len = 0;

    for(size_t i = 0; i < argc; i++) {
        if(!is_flag(argv[i], FALLBACK_FLAG)) {
            len++;
        } else {
            i++;
        }
    }

    return len;
}

void fallback_fill_argv(char **dest, int old_argc, char **old_argv, int new_argc) {
    size_t end = 0;
    
    for(size_t i = 0; i < old_argc; i++) {
        if(!is_flag(old_argv[i], FALLBACK_FLAG)) {
            dest[end] = old_argv[i];
            end++;
        } else {
            i++;
        }
    }
}

int fallback_run_self(int argc, char **argv) {
    size_t new_argc = get_argc_for_child(argc, argv);    
    char *new_argv[new_argc + 1];

    fallback_fill_argv(new_argv, argc, argv, new_argc);

    new_argv[new_argc] = NULL;

    pid_t child_pid = fork();

    if(child_pid == 0) {
        execvp(argv[0], new_argv);

        fprintf(stderr, "error: Failed to replace forked process for crash guard\n");
        exit(127);
    }

    if(child_pid < 0) {
        fprintf(stderr, "error: Unable to execute child process for crash guard\n");
        exit(1);
    }

    int status;
    waitpid(child_pid, &status, 0);

    return WEXITSTATUS(status);
}
