#define ES_VULKAN
#include "es_header.h"

static void resize_callback(es_window_t *window, i32_t width, i32_t height) {
    (void) window;
    printf("%d, %d\n", width, height);
}

static void key_callback(es_window_t *window, es_key_t keycode, usize_t mod, es_key_action_t action) {
    (void) window;
    (void) mod;
    (void) action;
    printf("%d: %d\n", action, keycode);
}

i32_t main(void) {
    es_window_t *window = es_window_init(800, 600, "1", true);
    es_window_set_resize_callback(window, resize_callback);
    es_window_set_key_callback(window, key_callback);

    while (es_window_is_open(window)) {
        es_window_poll_events(window);
    }

    es_window_free(window);

    return 0;
}
