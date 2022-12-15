#include "es_header.h"

i32_t main(void) {
    const char *file = "test.txt";

    es_file_write(file, "Intial write\n");
    es_file_write(file, "Write 2\n");

    char *content = es_file_read(file);
    printf("'%s'\n", content);
    es_free(content);

    return 0;
}
