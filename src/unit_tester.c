#include "es_header.h"

i32_t main(i32_t argc, char **argv) {
    es_da(const char *) src = NULL;
    for (i32_t i = 1; i < argc - 1; i++) {
        es_da_push(src, argv[i]);
    }

    const char *lib = argv[argc - 1];

    es_unit_test(src, lib);

    return 0;
}
