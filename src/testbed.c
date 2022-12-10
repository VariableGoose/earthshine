#include "es_header.h"

#include <stdio.h>

// TODO: Write an iterator for hash tables.
// TODO: Document everything. You'll thank yourself later.

i32_t main(void) {
    // Setup
    es_hash_table(char, i32_t) ht = NULL;
    /* es_hash_table_string_key(ht); */


    
    for (u8_t i = 0; i < 26; i++) {
        char key = 'a' + i;
        es_hash_table_insert(ht, key, i);
    }

    for (es_hash_table_iter_t iter = 0; es_hash_table_iter_valid(ht, iter); es_hash_table_iter_advance(ht, iter)) {
        i32_t value = es_hash_table_iter_get(ht, iter);
        char key = es_hash_table_iter_get_key(ht, iter);
        printf("%c: %d\n", key, value);
    }


    // Cleanup
    es_hash_table_free(ht);
    return 0;
}
