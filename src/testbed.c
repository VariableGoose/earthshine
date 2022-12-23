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
    (void) keycode;
    printf("%d: %d\n", action, keycode);
}

static void button_callback(es_window_t *window, es_button_t button, es_key_action_t action) {
    (void) window;
    printf("%d, %d\n", action, button);
}

static void cursor_position_callback(es_window_t *window, i32_t x, i32_t y) {
    (void) window;
    printf("%d, %d\n", x, y);
}

void scroll_callback(es_window_t *window, i32_t offset) {
    (void) window;
    printf("%d\n", offset);
}

i32_t main(void) {
    es_window_t *window = es_window_init(800, 600, "1", true);

    es_window_set_resize_callback(window, resize_callback);
    es_window_set_key_callback(window, key_callback);
    es_window_set_mouse_button_callback(window, button_callback);
    es_window_set_cursor_position_callback(window, cursor_position_callback);
    es_window_set_scroll_callback(window, scroll_callback);

    while (es_window_is_open(window)) {
        es_window_poll_events(window);
    }

    es_window_free(window);

    return 0;
}
