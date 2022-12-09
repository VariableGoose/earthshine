#include "es_header.h"

#include <stdio.h>

i32_t main(void) {
    es_da(i32_t) da = NULL;

    i32_t arr[5] = {0, 1, 2, 3, 4};
    /* es_da_insert_arr(da, arr, es_arr_len(arr), 0); */
    es_da_push_arr(da, arr, es_arr_len(arr));
    i32_t out[2] = {0};
    /* es_da_remove_arr(da, 2, 0, out); */
    /* es_da_pop_arr(da, 2, NULL); */

    for (usize_t i = 0; i < es_da_count(da); i++) {
        printf("%d\n", da[i]);
    }

    for (u32_t i = 0; i < es_arr_len(out); i++) {
        printf("out[%u] = %d\n", i, out[i]);
    }

    es_da_free(da);

    return 0;
}
