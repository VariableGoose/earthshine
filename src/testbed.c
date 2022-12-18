#define ES_VULKAN
#include "es_header.h"

static void resize_callback(es_window_t *window, i32_t width, i32_t height) {
    (void) window;
    printf("%d, %d\n", width, height);
}

i32_t main(void) {
    /* for (u32_t i = 0; i < 24; i++) { */
    /*     printf("case XK_F%d:\n    return ES_KEY_F%d;\n", i + 1, i + 1); */
    /* } */
    printf("%d\n", 1 << 7);

    es_window_t *window = es_window_init(800, 600, "1", true);
    es_window_callback_resize(window, resize_callback);
    es_window_callback_key(window, key_callback);

    while (es_window_is_open(window)) {
        es_window_poll_events(window);
    }

    es_window_free(window);

    return 0;
}
