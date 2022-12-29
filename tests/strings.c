#include "es_header.h"

es_unit(c_string_length) {
    es_unit_check(es_cstr_len("abcdef") == 6);
}

es_unit(c_string_compare_length) {
    es_unit_check(es_cstr_cmp_len("A\0B\0", "A\0B\0", 4) == 0 && es_cstr_cmp_len("AB", "AD", 2) == 'B' - 'D');
}

es_unit(c_string_compare) {
    es_unit_check(es_cstr_cmp("A\0B\0", "A\0") == 0 && es_cstr_cmp("AB", "AD") == 'B' - 'D');
}

es_unit(character_is_whitespace) {
    es_unit_check(es_is_whitespace(' ') && !es_is_whitespace('a'));
}

es_unit(character_is_numerical) {
    es_unit_check(es_is_digit('5') && !es_is_digit('a'));
}

es_unit(character_is_alphabetical) {
    es_unit_check(!es_is_alpha(' ') && es_is_alpha('d') && es_is_alpha('D'));
}

es_unit(string_create_length) {
    es_str_t str = es_strn("A\0B\0", 4);
    es_unit_check(str[0] == 'A' && str[1] == '\0' && str[2] == 'B' && str[3] == '\0');
}

es_unit(string_create) {
    es_str_t str = es_str("A\0B\0");
    b8_t success = (str[0] == 'A' && str[1] == '\0');
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_empty) {
    es_str_t str = es_str_empty();
    b8_t success = (es_cstr_len(str) == 0);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_length) {
    es_str_t str = es_str("abcdef");
    b8_t success = (es_str_len(str) == 6);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_reserve) {
    es_str_t str = es_str_reserve(8);
    b8_t success = (es_str_len(str) == 8);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_compare) {
    es_str_t str = es_str("abcd");
    b8_t success = (es_str_cmp(str, "abcd") == 0 && es_str_cmp(str, "abcdef") != 0);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_concatinate_length) {
    es_str_t str = es_str("abcdef");
    es_str_concat_len(&str, "ghijk", 3);
    b8_t success = (es_str_cmp(str, "abcdefghi") == 0);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_concatinate) {
    es_str_t str = es_str("abcdef");
    es_str_concat(&str, "ghijk");
    b8_t success = (es_str_cmp(str, "abcdefghijk") == 0);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_concatinate_character) {
    es_str_t str = es_str("abcdef");
    es_str_concat_char(&str, '3');
    es_str_concat_char(&str, '2');
    b8_t success = (es_str_cmp(str, "abcdef32") == 0);
    es_str_free(&str);
    es_unit_check(success);
}

es_unit(string_substring_length) {
    es_str_t sub = es_str_sub_len("abcdef", 2, 2);
    b8_t success = (es_str_cmp(sub, "cd") == 0);
    es_str_free(&sub);
    es_unit_check(success);
}

es_unit(string_substring_index) {
    es_str_t sub = es_str_sub_index("abcdef", 3, 5);
    b8_t success = (es_str_cmp(sub, "def") == 0);
    es_str_free(&sub);
    es_unit_check(success);
}

es_unit(string_substring_start) {
    es_str_t sub = es_str_sub_start("abcdef", 4);
    b8_t success = (es_str_cmp(sub, "abcd") == 0);
    es_str_free(&sub);
    es_unit_check(success);
}

es_unit(string_substring_end) {
    es_str_t sub = es_str_sub_end("abcdef", 4);
    b8_t success = (es_str_cmp(sub, "cdef") == 0);
    es_str_free(&sub);
    es_unit_check(success);
}

es_unit(string_split_by_delimiter) {
    es_da(es_str_t) list = es_str_split_by_delim("hello foo bar", ' ');
    b8_t success = (es_da_count(list) == 3);
    success = (es_str_cmp(list[0], "hello") == 0) && success;
    success = (es_str_cmp(list[1], "foo")   == 0) && success;
    success = (es_str_cmp(list[2], "bar")   == 0) && success;
    es_str_free_list(&list);
    es_unit_check(success);
}

es_unit(string_valid) {
    for (b8_t i = false; !i; i = true) {
    }

    es_str_t str = es_str("abcdef");
    b8_t valid = es_str_valid(str);
    es_str_free(&str);
    b8_t not_valid = es_str_valid(str);
    es_unit_check(valid && !not_valid);
}
