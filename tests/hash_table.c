#include "es_header.h"

#define ht_fill(HT) for (i32_t i = -4; i < 5; i++) { es_hash_table_insert(HT, i, i % 2 == 0); }

es_unit(hash_table_insert) {
    es_hash_table(i32_t, b8_t) ht = NULL;

    for (i32_t i = -4; i < 5; i++) {
        es_hash_table_insert(ht, i, i % 2 == 0);
    }

    // Check if an arbitrary entry has been added.
    b8_t entry_found = false;
    for (usize_t i = 0; i < es_da_count(ht->entries); i++) {
        if (ht->entries[i].key == 3) {
            entry_found = true;
            break;
        }
    }

    es_unit_check(ht->count == 9 && entry_found);
}

es_unit(hash_table_remove) {
    es_hash_table(i32_t, b8_t) ht = NULL;
    ht_fill(ht);
    
    es_hash_table_remove(ht, 3);

    // Check if entry has actually been removed.
    b8_t entry_found = false;
    for (usize_t i = 0; i < es_da_count(ht->entries); i++) {
        if (ht->entries[i].key == 3) {
            entry_found = true;
            break;
        }
    }

    es_unit_check(ht->count == 8 && !entry_found);
}

es_unit(hash_table_get) {
    es_hash_table(i32_t, b8_t) ht = NULL;
    ht_fill(ht);

    b8_t value = true;
    value = es_hash_table_get(ht, 3);

    es_unit_check(!value);
}

es_unit(hash_table_clear) {
    es_hash_table(i32_t, b8_t) ht = NULL;
    ht_fill(ht);

    es_hash_table_clear(ht);

    // Check for an arbitrary entry.
    b8_t entry_found = false;
    for (usize_t i = 0; i < es_da_count(ht->entries); i++) {
        if (ht->entries[i].key == 3) {
            entry_found = true;
            break;
        }
    }

    es_unit_check(!entry_found && ht->count == 0);
}

es_unit(hash_table_count) {
    es_hash_table(i32_t, b8_t) ht = NULL;
    ht_fill(ht);

    es_unit_check(es_hash_table_count(ht) == 9);
}

es_unit(hash_table_iteration) {
    es_hash_table(i32_t, b8_t) ht = NULL;
    ht_fill(ht);

    struct { i32_t key; b8_t value; b8_t found; } entries[9] = {
        { -4, true,  false },
        { -3, false, false },
        { -2, true,  false },
        { -1, false, false },
        {  0, true,  false },
        {  1, false, false },
        {  2, true,  false },
        {  3, false, false },
        {  4, true,  false },
    };

    for (es_hash_table_iter_t iter = es_hash_table_iter_new(ht); es_hash_table_iter_valid(ht, iter); es_hash_table_iter_advance(ht, iter)) {
        for (u8_t entry = 0; entry < es_arr_len(entries); entry++) {
            i32_t key   = es_hash_table_iter_get_key(ht, iter);
            b8_t  value = es_hash_table_iter_get(ht, iter);
            entries[entry].found = (entries[entry].key == key && entries[entry].value == value) || (entries[entry].found);
        }
    }

    for (u8_t i = 0; i < es_arr_len(entries); i++) {
        if (!entries[i].found) {
            es_unit_fail();
        }
    }

    es_unit_success();
}
