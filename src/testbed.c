#include "es_header.h"

#include <stdio.h>

void thread_proc(void *arg) {
    (void) arg;

    printf("Thread\n");
}

i32_t main(void) {
    es_da(es_thread_t) t = NULL;
    for (usize_t i = 0; i < 16; i++) {
        es_da_push(t, es_thread(thread_proc, NULL));
    }

    for (usize_t i = 0; i < es_da_count(t); i++) {
        es_thread_wait(t[i]);
    }

    return 0;
}
