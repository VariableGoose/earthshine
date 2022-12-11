#include "es_header.h"

#include <stdio.h>

#define COUNT_TO 1000000
#define MAX_CORES 16

es_mutex_t mutex;
u64_t i = 0;

void *count(void *arg) {
    (void) arg;

    for (;;) {
        if (i >= COUNT_TO) {
            return NULL;
        }

        i++;

        printf("i = %lld\n", i);
    }
}

void *count_sync(void *arg) {
    (void) arg;

    for (;;) {
        es_mutex_lock(&mutex);
        if (i >= COUNT_TO) {
            es_mutex_unlock(&mutex);
            return NULL;
        }

        i++;

        es_mutex_unlock(&mutex);
        printf("i = %lld\n", i);
    }
}

i32_t main(i32_t argc, char **argv) {
    (void) argv;
    es_thread_func_t func = argc > 1 ? count_sync : count;

    mutex = es_mutex_init();

    es_thread_t *thread_group = es_malloc(sizeof(es_thread_t) * MAX_CORES);

    for (u32_t j = 0; j < MAX_CORES; j++) {
        thread_group[j] = es_thread(func, NULL);
    }

    for (u32_t j = 0; j < MAX_CORES; j++) {
        es_thread_wait(thread_group[j], NULL);
    }

    es_mutex_free(&mutex);
    es_free(thread_group);

    return 0;
}
