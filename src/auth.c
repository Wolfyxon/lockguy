#include "auth.h"

AuthStatus auth_current_user(const char *password) {
    size_t username_max = sysconf(_SC_LOGIN_NAME_MAX);

    if(username_max < 0) { // max length unknown 
        username_max = USERNAME_ABSOLUTE_MAX;
    }

    char username[username_max];
    getlogin_r(username, sizeof(username) - 1);

    if(username[0] == '\0') {
        return AUTH_CANT_GET_USER;
    }

    return auth_user(username, password);
}

// Uses the su command to verify password. Prolly not the best idea but hey it works!
AuthStatus auth_user(const char *username, const char *password) {
    if(!check_shell_str(username)) {
        return AUTH_ILLEGAL_CHAR; // possible code execution
    }

    if(strlen(username) > USERNAME_ABSOLUTE_MAX) {
        return AUTH_USERNAME_TOO_LONG;
    }

    char cmd[512];
    snprintf(cmd, sizeof(cmd), "sh -c 'su %s -c echo' >/dev/null 2>&1", username);
    cmd[sizeof(cmd) - 1] = '\0';

    FILE *process_pipe = popen(cmd, "w");

    if(process_pipe == NULL) {
        return AUTH_CANT_RUN;
    }

    fwrite(password, 1, strlen(password), process_pipe);
    int status = pclose(process_pipe);

    if(status == 0) {
        return AUTH_SUCCESS;
    }

    return AUTH_FAIL;
}

bool check_shell_str(const char *str) {
    for(size_t i = 0; ; i++) {
        char str_c = str[i];

        if(str_c == '\0') {
            return true;
        }

        for(size_t j = 0; ; j++) {
            char unsafe_c = SHELL_UNSAFE_CHARS[j];
        
            if(unsafe_c == '\0') {
                break;
            }

            if(str_c == unsafe_c) {
                return false;
            }
        }
    }
}
