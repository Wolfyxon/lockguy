#include "utils.h"

bool is_flag(const char *arg, const char *flag_name) {
    size_t prefix_len = 2;
    size_t len = strlen(arg);
    
    if(len < prefix_len + 1) {
        return false;
    }

    if(strncmp(arg, "--", prefix_len) != 0) {
        return false;
    }

    return strcmp(arg + prefix_len, flag_name) == 0;
}

void zero_buf(char *buf, size_t len) {
    for(size_t i = 0; i < len; i++) {
        buf[i] = '\0';
    }
}

void zero_str(char *str) {
    return zero_buf(str, strlen(str));
}
