#define ES_VULKAN
#include "es_header.h"

i32_t main(void) {
    es_da(i32_t) da = NULL;
    for (i32_t i = -4; i < 5; i++) {
        es_da_push(da, i);
    }
    i32_t out = 0;
    es_da_pop(da, &out);
    
    if (out != 4 || es_memcmp(da, &(i32_t[8]) {-4, -3, -2, -1, 0, 1, 2, 3}, 8 * sizeof(i32_t)) != 0) {
        return 1;
    }

    return 0;
}
