#pragma once

typedef enum {
    COMPONENT_TEXT,
    COMPONENT_PASSWORD_INPUT,
    COMPONENT_CLOCK,
} ComponentType;

typedef struct {
    char *text;
} TextComponent;

typedef struct {} PasswordInputComponent;
typedef struct {} ClockComponent;

typedef struct {
    float x;
    float y;
    ComponentType type;

    union {
        TextComponent text;
        PasswordInputComponent pass_inp;
        ClockComponent clock;
    };
} Component;
