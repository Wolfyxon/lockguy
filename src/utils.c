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

void strcat_safe(char *dest, const char *src, size_t dest_size) {
    if (dest_size == 0) return;
    
    size_t dest_len = strlen(dest);
    size_t src_len = strlen(src);
    
    if (src_len >= dest_size - dest_len) {
        src_len = dest_size - dest_len - 1; 
    }

    memcpy(&dest[dest_len], src, src_len);
    dest[dest_len + src_len] = '\0';
}

void strcut_back(char *str, size_t n) {
    size_t len = strlen(str);
    
    for(size_t i = 0; i < n; i++) {
        str[len - 1 - i] = '\0';
    }
}

millis_t get_time_ms() {
    struct timeval time;
    gettimeofday(&time, NULL);
    
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}
