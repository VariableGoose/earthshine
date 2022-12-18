#include "es_header.h"

static void resize_callback(es_window_t *window, i32_t width, i32_t height) {
    (void) window;
    printf("%d, %d\n", width, height);
}

i32_t main(void) {
    es_window_t *window = es_window_init(800, 600, "1", true);
    es_window_t *window2 = es_window_init(800, 600, "2", true);
    es_window_callback_resize(window, resize_callback);

    while (es_window_is_open(window) && es_window_is_open(window2)) {
        es_window_poll_events(window);
        es_window_poll_events(window2);
    }

    es_window_free(window);
    es_window_free(window2);

    return 0;
}
