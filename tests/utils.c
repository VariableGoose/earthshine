#include "es_header.h"

es_unit(utility_array_length) {
    i32_t arr[4] = {0, 1, 2, 3};
    i32_t len = es_arr_len(arr);
    es_unit_check(len == 4);
}

es_unit(utility_offset_into_structure) {
    struct dummy_t { i32_t a; i32_t b; };
    i32_t offset = es_offset(struct dummy_t, b);
    es_unit_check(offset == 4);
}

es_unit(utility_max) {
    i32_t maxl = es_max(2, 1);
    i32_t maxr = es_max(3, 5);

    es_unit_check(maxl == 2 && maxr == 5);
}

es_unit(utility_min) {
    i32_t minl = es_min(3, 5);
    i32_t minr = es_min(2, 1);

    es_unit_check(minl == 3 && minr == 1);
}

es_unit(utility_clamp) {
    i32_t clamped_upper = es_clamp(4, -1, 1);
    i32_t clamped_lower = es_clamp(-2, -1, 2);
    i32_t clamped = es_clamp(0, -1, 1);

    es_unit_check(clamped_upper == 1 && clamped_lower == -1 && clamped == 0);
}

es_unit(utility_lerp) {
    f32_t lerped000 = es_lerp(0, 10, 0.00f);
    f32_t lerped025 = es_lerp(0, 10, 0.25f);
    f32_t lerped050 = es_lerp(0, 10, 0.50f);
    f32_t lerped075 = es_lerp(0, 10, 0.75f);
    f32_t lerped100 = es_lerp(0, 10, 1.00f);
    es_unit_check(lerped000 == 0.0f && lerped025 == 2.5f && lerped050 == 5.0f && lerped075 == 7.5 && lerped100 == 10.0f);
}

//
// TODO: Find a way to test hashing, get_time and sleep.
//
