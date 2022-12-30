/* #define ES_VULKAN */
#include "es_header.h"

void error_callback(es_error_t err) {
    es_log("{str}:{i32}: '{str}'\n", err.file, err.line, err.message);
}

i32_t main(void) {
    es_formatter_init();

    es_error_set_callback(error_callback);
    es_error_set_filter(ES_ERROR_SEVERITY_FATAL | ES_ERROR_SEVERITY_ERROR | ES_ERROR_SEVERITY_WARNING | ES_ERROR_SEVERITY_INFO);
    
    es_error("Fatal",   ES_ERROR_SEVERITY_FATAL);
    es_error("Warning", ES_ERROR_SEVERITY_WARNING);
    es_error("Error",   ES_ERROR_SEVERITY_ERROR);
    es_error("Info",    ES_ERROR_SEVERITY_INFO);
    es_error("This will not log.", ES_ERROR_SEVERITY_DEBUG);
    
    es_da(const char *) exts = NULL;
    es_window_get_extensions(&exts);
    es_log("{str}, {str}\n", exts[0], exts[1]);

    es_formatter_free();
    return 0;
}
