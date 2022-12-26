#include "es_header.h"

static void fill_da(es_da(i32_t) *da) {
    for (i32_t i = -4; i < 5; i++) {
        es_da_push(*da, i);
    }
}

es_unit(dynamic_array_insert) {
    es_da(i32_t) da = NULL;
    for (i32_t i = -4; i < 5; i++) {
        es_da_insert(da, i, 0);
    }

    es_unit_check(memcmp(da, &(i32_t[9]) {4, 3, 2, 1, 0, -1, -2, -3, -4}, 9 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_insert_fast) {
    es_da(i32_t) da = NULL;
    for (i32_t i = -4; i < 5; i++) {
        es_da_insert_fast(da, i, 0);
    }

    es_unit_check(memcmp(da, &(i32_t[9]) {4, -4, -3, -2, -1, 0, 1, 2, 3}, 9 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_insert_array) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    es_da_insert_arr(da, ((i32_t [4]) {1, 2, 3, 4}), 4, 0);

    es_unit_check(memcmp(da, &(i32_t [13]) {1, 2, 3, 4, -4, -3, -2, -1, 0, 1, 2, 3, 4}, 13 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_push) {
    es_da(i32_t) da = NULL;
    for (i32_t i = -4; i < 5; i++) {
        es_da_push(da, i);
    }

    es_unit_check(memcmp(da, &(i32_t [9]) {-4, -3, -2, -1, 0, 1, 2, 3, 4}, 9 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_push_array) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    es_da_push_arr(da, ((i32_t [4]) {1, 2, 3, 4}), 4);

    es_unit_check(memcmp(da, &(i32_t [13]) {-4, -3, -2, -1, 0, 1, 2, 3, 4, 1, 2, 3, 4}, 13 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_remove) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t out = 0;
    es_da_remove(da, 0, &out);

    es_unit_check(out == -4 && memcmp(da, &(i32_t [8]) {-3, -2, -1, 0, 1, 2, 3, 4}, 8 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_remove_fast) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t out = 0;
    es_da_remove_fast(da, 0, &out);

    es_unit_check(out == -4 && memcmp(da, &(i32_t [8]) {4, -3, -2, -1, 0, 1, 2, 3}, 8 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_remove_array) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t out[4] = {0};
    es_da_remove_arr(da, 4, 0, out);

    es_unit_check(memcmp(out, &(i32_t [4]) {-4, -3, -2, -1}, 4 * sizeof(i32_t)) == 0 && memcmp(da, &(i32_t [5]) {0, 1, 2, 3, 4}, 5 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_pop) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t out = 0;
    es_da_pop(da, &out);
    
    es_unit_check(out == 4 && memcmp(da, &(i32_t[8]) {-4, -3, -2, -1, 0, 1, 2, 3}, 8 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_pop_array) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t out[4] = {0};
    es_da_pop_arr(da, 4, out);

    es_unit_check(memcmp(out, &(i32_t [4]) {1, 2, 3, 4}, 4 * sizeof(i32_t)) == 0 && memcmp(da, &(i32_t [5]) {-4, -3, -2, -1, 0}, 5 * sizeof(i32_t)) == 0);
}

es_unit(dynamic_array_last) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    i32_t last = es_da_last(da);
    es_unit_check(last == 4);
}

es_unit(dynamic_array_count) {
    es_da(i32_t) da = NULL;
    fill_da(&da);

    es_unit_check(es_da_count(da) == 9);
}
