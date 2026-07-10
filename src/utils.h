#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include <sys/time.h>

typedef unsigned long millis_t;

bool is_flag(const char *arg, const char *flag_name);

void zero_buf(char *buf, size_t len);
void zero_str(char *str);
void strcat_safe(char *dest, const char *src, size_t dest_size);

millis_t get_time_ms();
