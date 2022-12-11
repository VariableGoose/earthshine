#include "es_header.h"

#include <stdio.h>

const char *keys[] = {"key0", "key1", "key2", "key3", "key4", "key5", "key6", "key7", "key8", "key9"};

i32_t main(void) {
    // Setup
    es_hash_table(const char *, const char *) ht = NULL;
    es_hash_table_string_key(ht);


    
    for (u8_t i = 0; i < es_arr_len(keys); i++) {
        es_hash_table_insert(ht, keys[i], i % 2 ? "Odd" : "Even");
    }

    es_hash_table_remove(ht, keys[2]);

    for (es_hash_table_iter_t iter = es_hash_table_iter_new(ht); es_hash_table_iter_valid(ht, iter); es_hash_table_iter_advance(ht, iter)) {
        const char *key = es_hash_table_iter_get_key(ht, iter);
        const char *value = es_hash_table_iter_get(ht, iter);
        printf("%s: %s\n", key, value);
    }



    // Cleanup
    es_hash_table_free(ht);
    return 0;
}
