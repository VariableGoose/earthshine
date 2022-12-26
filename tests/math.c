#include "es_header.h"

es_unit(floating_point_rounding) { es_unit_check(es_float_round(3.1415296f, 2) == 3.14f); }

/*=========================*/
// Vector 2
/*=========================*/

es_unit(vector2_multiplication) { es_unit_check(vec2_equal(vec2_mul(vec2(1, 2), vec2(4, 3)), vec2(4, 6))); }
es_unit(vector2_division) { es_unit_check(vec2_equal(vec2_div(vec2(1, 6), vec2(4, 3)), vec2(0.25f, 2))); }
es_unit(vector2_addition) { es_unit_check(vec2_equal(vec2_add(vec2(1, 2), vec2(4, 3)), vec2(5, 5))); }
es_unit(vector2_subtraction) { es_unit_check(vec2_equal(vec2_sub(vec2(1, 2), vec2(4, 3)), vec2(-3, -1))); }

es_unit(vector2_scaler_multiplication) { es_unit_check(vec2_equal(vec2_muls(vec2(1, 2), 4), vec2(4, 8))); }
es_unit(vector2_scaler_division) { es_unit_check(vec2_equal(vec2_divs(vec2(4, 6), 2), vec2(2, 3))); }
es_unit(vector2_scaler_addition) { es_unit_check(vec2_equal(vec2_adds(vec2(1, 2), 3), vec2(4, 5))); }
es_unit(vector2_scaler_subtraction) { es_unit_check(vec2_equal(vec2_subs(vec2(1, 2), 2), vec2(-1, 0))); }

es_unit(vector2_magnitude) { es_unit_check(es_float_round(vec2_magnitude(vec2(1, 1)), 6) == 1.414214f); }

es_unit(vector2_noramlize) {
    vec2_t norm = vec2_normalize(vec2s(1));
    norm.x = es_float_round(norm.x, 6);
    norm.y = es_float_round(norm.y, 6);
    es_unit_check(vec2_equal(norm, vec2s(0.707107f)));
}

es_unit(vector2_dot_product) { es_unit_check(vec2_dot(vec2(1, 2), vec2(3, 4)) == 11); }
es_unit(vector2_cross_product) { es_unit_check(vec2_cross(vec2(1, 2), vec2(3, 4)) == -2); }
