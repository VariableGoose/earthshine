#include "es_header.h"

i32_t main(i32_t argc, char **argv) {
    es_da(const char *) src = NULL;
    for (i32_t i = 1; i < argc - 1; i++) {
        es_da_push(src, argv[i]);
    }

    const char *lib = argv[argc - 1];

    es_unit_test_t test = es_unit_test(src, lib);

    printf("\033[0;92mSuccessful: %lu\033[0m\n", es_da_count(test.success));
    printf("\033[0;91mFailures:   %lu\033[0m\n", es_da_count(test.fail));
    printf("\033[0;93mNot found:  %lu\033[0m\n", es_da_count(test.not_found));

    for (usize_t i = 0; i < es_da_count(test.not_found); i++) {
        printf("Unit '%s' not found.\n", test.not_found[i]);
    }

    for (usize_t i = 0; i < es_da_count(test.fail); i++) {
        es_unit_result_t r = test.fail[i];
        printf("\033[0;91mFail: %s\n    File: '%s'\n    Line: %d\033[0m\n", r.func, r.file, r.line);
    }

    es_unit_test_free(&test);

    return 0;
}
