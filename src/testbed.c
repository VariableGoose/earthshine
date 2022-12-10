#include "es_header.h"

#include <stdio.h>

typedef struct test_t {
    i32_t a;
    f64_t b;
    b8_t c;
} test_t;

i32_t main(void) {
    es_hash_table(i32_t, i32_t) ht = NULL;

    for (usize_t i = 0; i < 255; i++) {
        es_hash_table_insert(ht, i, 255 - i);
    }

    i32_t val = es_hash_table_get(ht, 254);
    printf("%d\n", val);

    es_hash_table_free(ht);

    return 0;
}
