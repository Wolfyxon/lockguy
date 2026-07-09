#pragma once

#include <string.h>
#include <stdio.h>
#include <stdbool.h>

bool is_flag(const char *arg, const char *flag_name);

void zero_buf(char *buf, size_t len);
void zero_str(char *str);
