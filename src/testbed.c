#include "es_header.h"

#include <stdio.h>

i32_t main(void) {
    es_da(i32_t) da = NULL;

    for (u32_t i = 0; i < 16; i++) {
        es_da_push(da, i);
    }

    for (usize_t i = 0; i < es_da_count(da); i++) {
        printf("%d\n", da[i]);
    }

    es_da_free(da);

    return 0;
}
