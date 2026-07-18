#include "tests.h"

#ifdef ENABLE_TESTS

void test_strcat_safe() {
    char str[128] = "hello ";
    strcat_safe(str, "world", sizeof(str));

    if(strcmp(str, "hello world") != 0) {
        fprintf(stderr, "error: '%s' != 'hello world'\n", str);
        exit(1);
    }
}

void test_load_font() {
    AppState state = {};
    x11_init_minimal(&state);

    XftFont *default_font = x11_load_font(&state, DEFAULT_FONT, DEFAULT_FONT_SIZE);
    XftFont *other_font = x11_load_font(&state, "Times New Roman", 124);
    XftFont *fallback = x11_load_font(&state, "uhfhrfoijpoqpokpopodeofu", 432);

    if(default_font == NULL) {
        fprintf(stderr, "error: x11_load_font() failed for default\n");
        exit(1);
    }

    if(other_font == NULL) {
        fprintf(stderr, "error: x11_load_font() failed for ''Times New Roman''\n");
        exit(1);
    }

    if(fallback == NULL) {
        fprintf(stderr, "error: x11_load_font() returned NULL instead of fallback for nonexistent font\n");
        exit(1);
    }

    Display *disp = state.ctx.x11.display;

    XftFontClose(disp, default_font);
    XftFontClose(disp, other_font);
    XftFontClose(disp, fallback);

    x11_cleanup(&state);
}

void test_load_color() {
    AppState state = {};
    x11_init_minimal(&state);

    char *valid[] = {"#ffffff", "red"};
    char *mixed[] = {"##", "bababa", "white", "#ff0000", "101001010101"};

    for(size_t i = 0; i < sizeof(valid) / sizeof(char *); i++) {
        XftColor color = {0};

        if(!x11_try_load_color(&state, valid[i], &color)) {
            fprintf(stderr, "error: x11_try_load_color() failed for '%s'\n", valid[i]);
            exit(1);
        }

        x11_free_color(&state, &color);
    }

    for(size_t i = 0; i < sizeof(mixed) / sizeof(char *); i++) {
        XftColor color = x11_load_color(&state, mixed[i]); // will error if font is not found and fallback can't be loaded
        x11_free_color(&state, &color);
    }

    x11_cleanup(&state);
}

void run_tests() {
    exec_test(test_strcat_safe);
    exec_test(test_load_font);
    exec_test(test_load_color);
    
    puts("All tests successful");
    exit(0);
}

#endif