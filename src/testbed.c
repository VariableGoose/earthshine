#define ES_VULKAN
#include "es_header.h"

i32_t main(void) {
    es_da(const char *) files = NULL;
    es_da_push(files, (const char *) "tests/debug.c");
    
    es_unit_test(files, "bin/unit_tests.dll");

    return 0;
}
