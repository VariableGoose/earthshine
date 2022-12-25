#include "es_header.h"

es_unit(success) {
    es_unit_success();
}

es_unit_result_t es_unit_test_fail(const char *func_name) {
    es_unit_fail();
}

/* es_unit(fail) { */
/*     es_unit_fail(); */
/* } */
