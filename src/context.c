#include "context.h"

void state_init_dynamic(AppState *state) {
    state_init_password_buf(state);
}

void state_init_password_buf(AppState *state) {
    state->password_max_len = sysconf(_SC_PASS_MAX);

    if(state->password_max_len < 1) {
        state->password_max_len = PASSWORD_MAX_LEN_DEFAULT;
    }

    state->password_buf = calloc(state->password_max_len, 1);

    if(state->password_buf == NULL) {
        fprintf(stderr, "error: Unable to allocate memory for password input\n");
        exit(1);
    }
}

void clear_password(AppState *state) {
    for(size_t i = 0; i < state->password_max_len; i++) {
        state->password_buf[i] = '\0';
    }
}
