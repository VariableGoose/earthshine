#include "es_header.h"

#include <stdio.h>

i32_t main(void) {
    // Setup
    es_hash_table(char, i32_t) ht = NULL;
    /* es_hash_table_string_key(ht); */


    
    for (u8_t i = 0; i < 26; i++) {
        char key = 'a' + i;
        es_hash_table_insert(ht, key, i);
    }

    for (usize_t i = 0; i < es_da_count(ht->entries); i++) {
        printf("%c: %d\n", ht->entries[i].key, ht->entries[i].value);
    }


    // Cleanup
    es_hash_table_free(ht);
    return 0;
}
