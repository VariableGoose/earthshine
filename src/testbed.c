#include "es_header.h"

i32_t main(void) {
    m2_t m = m2f(4, 7, 2, 6);
    m2_t m_inv = m2_inv(m);
    m2_t identity = m2_mul(m, m_inv);

    printf("%f, %f\n%f, %f\n", identity.i.x, identity.i.y, identity.j.x, identity.j.y);

    return 0;
}
