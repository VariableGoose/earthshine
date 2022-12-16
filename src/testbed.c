#include "es_header.h"

i32_t main(void) {
    mat4_t m = mat4f(
         1,  1,  1, -1,
         1,  1, -1,  1,
         1, -1,  1,  1,
        -1,  1,  1,  1
    );
    mat4_t inv = mat4_inverse(m);
    mat4_t res = mat4_mul(m, inv);
    f32_t *f = (f32_t *) &res;

    for (u8_t x = 0; x < 4; x++) {
        for (u8_t y = 0; y < 4; y++) {
            printf("%5.2f", f[x + y * 4]);
        }
        printf("\n");
    }

    return 0;
}
