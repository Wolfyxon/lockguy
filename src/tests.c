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

void test_list_font_sizes() {
    AppState state = {
        .ctx_type = DISPLAY_CTX_X11
    };

    x11_init_minimal(&state);

    size_t found_count = 0;

    printf("Found font sizes:\n");

    for(size_t i = 0; i < 200; i++) {
        XFontStruct *font = x11_get_font_with_size_exact(&state, i);

        if(font != NULL) {
            printf("%ld ", i);
            XFreeFont(state.ctx.x11.display, font);;

            found_count++;
        }
    }

    printf("\nTotal: %ld\n", found_count);
    x11_cleanup(&state);
}

void test_get_fonts() {
    AppState state = {
        .ctx_type = DISPLAY_CTX_X11
    };

    x11_init_minimal(&state);

    for(size_t i = 0; i < 100; i++) {
        XFontStruct *font = x11_try_get_font_with_size(&state, i);

        if(font == NULL) {
            fprintf(stderr, "error: x11_get_font_with_size() failed for size: %ld\n", i);
            exit(1);
        }
        
        XFreeFont(state.ctx.x11.display, font);
    }

    x11_cleanup(&state);
}

void run_tests() {
    exec_test(test_strcat_safe);
    exec_test(test_list_font_sizes);
    exec_test(test_get_fonts);

    puts("All tests successful");
    exit(0);
}

#endif