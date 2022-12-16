#include "es_header.h"

i32_t main(void) {
    m4_t m = m4f(
         1,  1,  1, -1,
         1,  1, -1,  1,
         1, -1,  1,  1,
        -1,  1,  1,  1
    );
    m4_t inv = m4_inv(m);
    m4_t res = m4_mul(m, inv);
    f32_t *f = (f32_t *) &res;

    for (u8_t x = 0; x < 4; x++) {
        for (u8_t y = 0; y < 4; y++) {
            printf("%5.2f", f[x + y * 4]);
        }
        printf("\n");
    }

    return 0;
}
