#include "es_header.h"

i32_t main(void) {
    es_window_t *window = es_window_init(800, 600, "1", false);
    es_window_t *window2 = es_window_init(800, 600, "2", true);

    while (es_window_is_open(window)) {
        es_window_poll_events(window);
        es_window_poll_events(window2);
    }

    es_window_free(window);
    es_window_free(window2);

    return 0;
}
