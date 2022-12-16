#include "es_header.h"

i32_t main(void) {
    m3_t m = m3f(
        3,  2,  0,
        0,  0,  1,
        2, -2, 1
    );
    m3_t inv = m3_inv(m);
    m3_t res = m3_mul(m, inv);
    f32_t *f = (f32_t *) &res;

    for (u8_t x = 0; x < 3; x++) {
        for (u8_t y = 0; y < 3; y++) {
            printf("%3.f", f[x + y * 3]);
        }
        printf("\n");
    }

    return 0;
}
