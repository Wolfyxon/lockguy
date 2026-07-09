#pragma once

#include <unistd.h>

#include "utils.h"

#define USERNAME_ABSOLUTE_MAX 256

#define SHELL_UNSAFE_CHARS "\\\"\'&$() "

#define AUTH_SHELL "sh -c"
#define AUTH_SHELL_POST ">/dev/null 2>&1"
#define AUTH_BIN "su"
#define AUTH_CMD_ARGS "-c echo"

typedef enum  {
    AUTH_SUCCESS,
    AUTH_DENIED,
    AUTH_FAIL,
    AUTH_CANT_RUN,
    AUTH_IO_ERROR,
    AUTH_ILLEGAL_CHAR,
    AUTH_USERNAME_TOO_LONG,
    AUTH_INVALID_PARAM,
    AUTH_CANT_GET_USER
} AuthStatus;

AuthStatus auth_user(const char* username, const char *password);
AuthStatus auth_current_user(const char *password);
bool check_shell_str(const char *str);