#include "es_header.h"

#define FILE "./tests/filesystem.test"

es_unit(filesystem_exists) {
    es_unit_check(es_file_exists(FILE));
}

es_unit(filesystem_read) {
    es_str_t content = es_file_read(FILE);
    es_unit_check(es_str_cmp(content, "Initial write\nAppend 1\nAppend 2") == 0);
}

es_unit(filesystem_write) {
    b8_t success = es_file_write(FILE, "Initial write\n");
    es_str_t content = es_file_read(FILE);
    es_unit_check(success && es_str_cmp(content, "Initial write\n") == 0);
}

es_unit(filesystem_append) {
    b8_t success = es_file_append(FILE, "Append 1\n");
    es_str_t content = es_file_read(FILE);
    b8_t match_after_first_append = (es_str_cmp(content, "Initial write\nAppend 1\n") == 0);
    es_str_free(&content);
    success = es_file_append(FILE, "Append 2") && success;
    content = es_file_read(FILE);
    es_unit_check(success && match_after_first_append && es_str_cmp(content, "Initial write\nAppend 1\nAppend 2") == 0);
}
