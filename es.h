/*====================================================================================================
    * Copyright: Linus Erik Pontus Kåreblom
    * Earthshine: A general purpose single header library
    * File: es.h
    * Version: 1.2
    * Github: https://github.com/linusepk/earthshine

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2022, Linus Erik Pontus Kåreblom

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
====================================================================================================*/

#ifndef ES_H
#define ES_H

#include <string.h>

/*=========================*/
// Context cracking
/*=========================*/

// Linux
#if defined(__linux__)
#define ES_OS_LINUX
#endif // __linux__

// Windows
#if defined(_WIN32) || defined(CYGWIN)
#define ES_OS_WIN32
#endif // _WIN32, CYGWIN

/*=========================*/
// API macros
/*=========================*/

#define ES_API extern
#define ES_GLOBAL extern
#define ES_INLINE static inline

/*=========================*/
// Basic typedefs
/*=========================*/

// Unsigned ints
typedef unsigned char      u8_t;
typedef unsigned short     u16_t;
typedef unsigned int       u32_t;
typedef unsigned long long u64_t;
typedef unsigned long      usize_t;

// Signed integers
typedef signed char      i8_t;
typedef signed short     i16_t;
typedef signed int       i32_t;
typedef signed long long i64_t;
typedef signed long      isize_t;

// Floats
typedef float  f32_t;
typedef double f64_t;

// Bools
typedef u8_t  b8_t;
typedef u32_t b32_t;

#ifndef true
#define true 1
#endif // true

#ifndef false
#define false 0
#endif // false

#ifndef NULL
#define NULL ((void *) 0)
#endif // NULL

/*=========================*/
// User defines
/*=========================*/

#ifndef es_malloc
#define es_malloc malloc
#include <malloc.h>
#endif // es_malloc

#ifndef es_calloc
#define es_calloc calloc
#include <malloc.h>
#endif // es_calloc

#ifndef es_realloc
#define es_realloc realloc
#include <malloc.h>
#endif // es_realloc

#ifndef es_free
#define es_free free
#include <malloc.h>
#endif // es_free

/*=========================*/
// Dynamic array
/*=========================*/

// Header struct placed before dynamic array.
typedef struct _es_da_header_t {
    // Entry count.
    usize_t count;
    // Max entry count.
    usize_t cap;
    // Size of entry.
    usize_t size;
} _es_da_header_t;

// Initial size of dynamic array.
#define _ES_DA_INIT_CAP 8

// Get the head of dynamic array from a pointer.
#define _es_da_head(P) ((_es_da_header_t *) ((u8_t *) (P) - sizeof(_es_da_header_t)))
// Get the start of dynamic array from a head.
#define _es_da_ptr(H) ((void *) ((u8_t *) (H) + sizeof(_es_da_header_t)))

// Initialize a new dynamic array.
ES_API void _es_da_init(void **arr, usize_t size);
// Free dynamic array.
ES_API void _es_da_free_impl(void **arr);

// Insert an entry into dynamic array.
ES_API void _es_da_insert_impl(void **arr, const void *data, usize_t index);
// Remove an entry from dynamic array.
ES_API void _es_da_remove_impl(void **arr, usize_t index, void *output);

// Insert an entry into dynamic array disregarding the order.
ES_API void _es_da_insert_fast_impl(void **arr, const void *data, usize_t index);
// Remove an entry from dynamic array disregarding the order.
ES_API void _es_da_remove_fast_impl(void **arr, usize_t index, void *output);

// Insert whole array into dynamic array.
ES_API void _es_da_insert_arr_impl(void **arr, const void *data, usize_t count, usize_t index);
// Remove whole array from dynamic array.
ES_API void _es_da_remove_arr_impl(void **arr, usize_t count, usize_t index, void *output);

// Resize dynamic array, if needed, according to insertion/removal count.
ES_API void _es_da_resize(void **arr, isize_t count);

// Get the amount of entries in  dynamic array.
ES_API usize_t es_da_count(void *arr);

// Declare dyanmic array.
#define es_da(T) T *

// Free a dynamic array.
#define es_da_free(ARR) _es_da_free_impl((void **) &(ARR))

// Insert an entry into dynamic array.
#define es_da_insert(ARR, D, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
// Remove an entry from dynamic array.
#define es_da_remove(ARR, I, OUT) _es_da_remove_impl((void **) &(ARR), I, (OUT))

// Insert an entry into dynamic array disregarding the order.
#define es_da_insert_fast(ARR, D, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_fast_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
// Remove an entry from dynamic array disregarding the order.
#define es_da_remove_fast(ARR, I, OUT) _es_da_remove_fast_impl((void **) &(ARR), (I), (OUT))

// Insert entry at the end of dynamic array.
#define es_da_push(ARR, D) es_da_insert_fast(ARR, D, es_da_count((ARR)));
// Remove entry at the end of dynamic array.
#define es_da_pop(ARR, OUT) es_da_remove_fast(ARR, es_da_count((ARR)) - 1, OUT);

// Insert a whole array into dynamic array.
#define es_da_insert_arr(ARR, D, C, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    _es_da_insert_arr_impl((void **) &(ARR), (D), (C), (I)); \
} while (0)
// Remove a whole array from dynamic array.
#define es_da_remove_arr(ARR, C, I, OUT) _es_da_remove_arr_impl((void **) &(ARR), (C), (I), (OUT))

// Insert array at the end of dynamic array.
#define es_da_push_arr(ARR, D, C) es_da_insert_arr(ARR, D, C, es_da_count((ARR)))
// Remove array from the end of dynamic array.
#define es_da_pop_arr(ARR, C, OUT) es_da_remove_arr(ARR, C, es_da_count((ARR)) - (C), OUT)

/*=========================*/
// Assert
/*=========================*/

// Abort when expresion is false.
ES_API void _es_assert_impl(const char *file, u32_t line, const char *expr_str, b8_t expr, const char *fmt, ...);

// Abort when expresion is false.
#define es_assert(EXPR, FMT, ...) _es_assert_impl(__FILE__, __LINE__, #EXPR, (EXPR), FMT, ##__VA_ARGS__)

/*=========================*/
// Utils
/*=========================*/

// Get length of an array.
#define es_arr_len(ARR) ((sizeof(ARR) / sizeof((ARR)[0])))

// Get stride of a memebr of a struct.
#define es_offset(S, M) ((usize_t) &(((S *) 0)->M))

#ifndef ES_SIPHASH_C_ROUNDS
#define ES_SIPHASH_C_ROUNDS 1
#endif // ES_SIPHASH_C_ROUNDS
#ifndef ES_SIPHASH_D_ROUNDS
#define ES_SIPHASH_D_ROUNDS 1
#endif // ES_SIPHASH_D_ROUNDS

// Hash string.
ES_API usize_t es_hash_str(const char *str);
// Hash any data (except string).
ES_API usize_t es_siphash(const void *data, usize_t len, usize_t seed);

/*=========================*/
// Hash table
/*=========================*/

#define ES_HASH_TABLE_MAX_CAP 0.75f
#define ES_HASH_TABLE_SEED 0x2664424Cul

// State of entry.
typedef enum _es_hash_table_entry_state_t {
    _ES_HASH_TABLE_ENTRY_DEAD  = 0,
    _ES_HASH_TABLE_ENTRY_ALIVE = 1,
} _es_hash_table_entry_state_t;

// Create entry struct from key and value.
#define _es_hash_table_entry(K, V) struct { \
    K key; \
    V value; \
    _es_hash_table_entry_state_t state; \
    usize_t hash; \
}

// Create hash table struct from key and value.
#define es_hash_table(K, V) struct { \
    es_da(_es_hash_table_entry(K, V)) entries; \
    _es_hash_table_entry(K, V) *temp_entry; \
    K temp_key; \
    b8_t string_key; \
    usize_t count; \
} *

// Initialize hash table.
#define _es_hash_table_init(HT) do { \
    if ((HT) != NULL) { \
        break; \
    } \
    usize_t ht_size = sizeof(*(HT)); \
    (HT) = es_malloc(ht_size); \
    memset((HT), 0, ht_size); \
    (HT)->entries = NULL; \
    es_da_push_arr((HT)->entries, NULL, 8); \
    (HT)->string_key = false; \
    (HT)->temp_entry = NULL; \
    (HT)->count = 0; \
} while (0)

// Hash table uses string as key.
#define es_hash_table_string_key(HT) do { \
    _es_hash_table_init(HT); \
    (HT)->string_key = true; \
} while (0)

// Backend insertion into entries
#define _es_hash_table_insert_entry(COUNT, ENTRIES, HASH, CAP, K, V) do { \
    usize_t index = (HASH) % (CAP); \
    usize_t count = 0; \
    usize_t search_hash = (ENTRIES)[index].hash; \
    for (;;) { \
        /* Stop if iteration has surpassed the cap, the hashes match or the entry is dead. */ \
        if (count >= CAP || search_hash == (HASH) || (ENTRIES)[index].state == _ES_HASH_TABLE_ENTRY_DEAD) { \
            break; \
        } \
        /* Increment. */ \
        index = (index + 1) % (CAP); \
        search_hash = (ENTRIES)[index].hash; \
        count++; \
    } \
    /* If it's a new entry, add it to entry count. */ \
    if ((ENTRIES)[index].state == _ES_HASH_TABLE_ENTRY_DEAD) { \
        (COUNT)++; \
    } \
    /* Initialize the entry. */ \
    (ENTRIES)[index].key   = (K); \
    (ENTRIES)[index].value = (V); \
    (ENTRIES)[index].state = _ES_HASH_TABLE_ENTRY_ALIVE; \
    (ENTRIES)[index].hash  = (HASH); \
} while(0)

// Resize hash table when needed.
#define _es_hash_table_resize(HT) do { \
    _es_hash_table_init(HT); \
    es_da(__typeof__(*((HT)->entries))) new_entries = NULL; \
    usize_t cap = es_da_count((HT)->entries); \
    /* If entry count is at the capacity limit, resize. */ \
    if ((HT)->count >= cap * ES_HASH_TABLE_MAX_CAP) { \
        es_da_push_arr(new_entries, NULL, cap * 2); \
    } \
    /* Resize isn't necessary. */ \
    if (new_entries == NULL) { \
        break; \
    } \
    (HT)->count = 0; \
    /* Remap every entry into the new entry array. */ \
    for (usize_t i = 0; i < cap; i++) { \
        __typeof__(*((HT)->entries)) curr = (HT)->entries[i]; \
        if (curr.state == _ES_HASH_TABLE_ENTRY_DEAD) { \
            continue; \
        } \
        _es_hash_table_insert_entry((HT)->count, new_entries, curr.hash, es_da_count(new_entries), curr.key, curr.value); \
    } \
    /* Free unused arrays. */ \
    es_da_free((HT)->entries); \
    /* Update hash table arrays. */ \
    (HT)->entries = new_entries; \
} while (0)

// Insert entry into hash table.
#define es_hash_table_insert(HT, K, V) do { \
    _es_hash_table_resize(HT); \
    __typeof__((HT)->entries->key) es_ht_temp_key = (K); \
    usize_t hash = _es_hash_table_hash_key((HT)->string_key, (void **) &es_ht_temp_key, sizeof((HT)->temp_key)); \
    usize_t cap = es_da_count((HT)->entries); \
    _es_hash_table_insert_entry((HT)->count, (HT)->entries, hash, cap, K, V); \
} while (0)

// Get entry value from hash table.
#define es_hash_table_get(HT, K) ( \
    (HT)->temp_key = (K), \
    (HT)->entries[ \
        _es_hash_table_get_index( \
        _es_hash_table_hash_key((HT)->string_key, (void **) &(HT)->temp_key, sizeof((HT)->temp_key)), \
            (void **) &(HT)->entries, \
            es_da_count((HT)->entries), \
            sizeof(*(HT)->entries), \
            es_offset(__typeof__(*(HT)->entries), hash), \
            es_offset(__typeof__(*(HT)->entries), state) \
        ) \
    ].value \
)

// Remove entry from hash table.
#define es_hash_table_remove(HT, K) do { \
    if ((HT) == NULL) { \
        break; \
    } \
    (HT)->temp_key = (K); \
    (HT)->temp_entry = (__typeof__((HT)->temp_entry)) &(HT)->entries[_es_hash_table_get_index( \
        _es_hash_table_hash_key((HT)->string_key, (void **) &(HT)->temp_key, sizeof((HT)->temp_key)), \
        (void **) &(HT)->entries, \
        es_da_count((HT)->entries), \
        sizeof(*(HT)->entries), \
        es_offset(__typeof__(*(HT)->entries), hash), \
        es_offset(__typeof__(*(HT)->entries), state) \
    )]; \
    memset((HT)->temp_entry, 0, sizeof(*(HT)->entries)); \
    (HT)->count--; \
} while (0)

// Reset hash table.
#define es_hash_table_clear(HT) do { \
    /* Recreate hash table arrays. */ \
    __typeof__((HT)->entries) new_entries = NULL; \
    es_da_push_arr(new_entries, NULL, 8); \
    es_da_free((HT)->entries); \
    (HT)->entries = new_entries; \
} while (0)

// Free allocated memory for hash table.
#define es_hash_table_free(HT) do { \
    if ((HT) == NULL) { \
        break; \
    } \
    es_da_free((HT)->entries); \
    es_free(HT); \
} while (0) \

// Get entry count in hash table.
#define es_hash_table_count(HT) (HT)->count

// Hash table iterator type.
typedef usize_t es_hash_table_iter_t;
#define es_hash_table_iter_new(HT) _es_hash_table_iter_new_impl( \
    (HT)->entries, \
    es_offset(__typeof__(*(HT)->temp_entry), state), \
    sizeof(__typeof__(*(HT)->temp_entry)), \
    es_da_count((HT)->entries) \
)
// Check if the hash table iterator valid.
#define es_hash_table_iter_valid(HT, IT) ((HT) == NULL ? false : ((IT) < es_da_count((HT)->entries)))
// Advance hash table iterator.
#define es_hash_table_iter_advance(HT, IT) _es_hash_table_iter_advance_impl( \
    es_offset(__typeof__(*(HT)->temp_entry), state), \
    sizeof(*(HT)->temp_entry), \
    (void *) (HT)->entries, \
    &(IT), \
    es_da_count((HT)->entries) \
)
// Get value of entry at current iterator index.
#define es_hash_table_iter_get(HT, IT) (HT)->entries[(IT)].value
// Get key of entry at current iterator index.
#define es_hash_table_iter_get_key(HT, IT) (HT)->entries[(IT)].key
// Remove an entry at iterator index.
#define es_hash_table_iter_remove(HT, IT) memset(&(HT)->entries[(IT)], 0, sizeof(__typeof__(*(HT)->temp_entry)))

// Get index of entry with wanted_hash. If not found, return a dead entry.
ES_API usize_t _es_hash_table_get_index(usize_t wanted_hash, void **entries, usize_t entry_count, usize_t entry_size, usize_t hash_offset, usize_t state_offset);
// Hash a hash table value. This only exists to avoid some compiler warnings.
ES_API usize_t _es_hash_table_hash_key(b8_t is_string, void **ptr, usize_t len);
// Get the first alive entry to start iteration.
ES_API usize_t _es_hash_table_iter_new_impl(void *entries, usize_t state_stride, usize_t entry_size, usize_t cap);
// Skip all dead entries, advancing the iteration.
ES_API void _es_hash_table_iter_advance_impl(usize_t state_stride, usize_t entry_size, const void *entries, usize_t *iter, usize_t cap);

/*=========================*/
// Implementation
/*=========================*/

#ifdef ES_IMPL
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/*=========================*/
// Dynamic array
/*=========================*/

void _es_da_init(void **arr, usize_t size) {
    if (*arr != NULL || size == 0) {
        return;
    }

    _es_da_header_t *head = es_malloc(sizeof(_es_da_header_t) + size * _ES_DA_INIT_CAP);

    head->cap = _ES_DA_INIT_CAP;
    head->size = size;
    head->count = 0;

    *arr = _es_da_ptr(head);
}

void _es_da_insert(void **arr, const void *data, usize_t index) {
    es_assert(arr != NULL, "Can't insert into a NULL array.", NULL);
    es_assert(data != NULL, "Can't insert NULL data into array.", NULL);

    _es_da_resize(arr, 1);
    _es_da_header_t *head = _es_da_head(*arr);

    // Out of bounds correction
    if (index > head->count) {
        index = head->count;
    }

    u8_t *ptr = *arr;
    void *src = ptr + (index) * head->size;
    void *dest = ptr + (index + 1) * head->size;

    memcpy(dest, src, (head->count - index) * head->size);
    memcpy(src, data, head->size);
    head->count++;
}

void _es_da_remove_impl(void **arr, usize_t index, void *output) {
    es_assert(arr != NULL, "Can't remove from a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (index + 1) * head->size;
    void *dest = ptr + (index) * head->size;

    if (output) {
        memcpy(output, dest, head->size);
    }

    memcpy(dest, src, (head->count - index - 1) * head->size);

    _es_da_resize(arr, -1);
    head->count--;
}

void _es_da_insert_fast_impl(void **arr, const void *data, usize_t index) {
    es_assert(arr != NULL, "Can't fast insert into a NULL array.", NULL);
    es_assert(data != NULL, "Can't fast insert NULL data into array.", NULL);

    _es_da_resize(arr, 1);
    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (index) * head->size;
    void *dest = ptr + head->count * head->size;

    memcpy(dest, src, head->size);
    memcpy(src, data, head->size);
    head->count++;
}

void _es_da_remove_fast_impl(void **arr, usize_t index, void *output) {
    es_assert(arr != NULL, "Can't remove from a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (head->count - 1) * head->size;
    void *dest = ptr + (index) * head->size;

    if (output) {
        memcpy(output, dest, head->size);
    }

    memcpy(dest, src, head->size);

    _es_da_resize(arr, -1);
    head->count--;
}

void _es_da_insert_arr_impl(void **arr, const void *data, usize_t count, usize_t index) {
    es_assert(arr != NULL, "Can't insert into a NULL array.", NULL);

    _es_da_resize(arr, count);
    _es_da_header_t *head = _es_da_head(*arr);
    u8_t *ptr = *arr;
    void *src = ptr + index * head->size;
    void *dest = ptr + (index + count) * head->size;

    memcpy(dest, src, (head->count - index) * head->size);
    if (data != NULL) {
        memcpy(src, data, head->size * count);
    } else {
        memset(src, 0, head->size * count);
    }
    head->count += count;
}

void _es_da_remove_arr_impl(void **arr, usize_t count, usize_t index, void *output) {
    es_assert(arr != NULL, "Can't insert into a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (index + count) * head->size;
    void *dest = ptr + index * head->size;

    if (output != NULL) {
        memcpy(output, ptr + index * head->size, head->size * count);
    }

    memcpy(dest, src, (head->count - index - count) * head->size);
    _es_da_resize(arr, -count);

    head->count -= count;
}

void _es_da_free_impl(void **arr) {
    es_assert(arr != NULL, "Can't free a NULL pointer.", NULL);

    if (*arr == NULL) {
        return;
    }
    es_free(_es_da_head(*arr));
}

void _es_da_resize(void **arr, isize_t count) {
    es_assert(arr != NULL, "Can't resize a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);
    // Double the capacity when full and inserting
    if (count > 0 && head->count + count >= head->cap) {
        while (head->count + count >= head->cap) {
            head->cap <<= 1;
        }
    }
    // Half capacity when half empty and removing
    else if (count < 0 && head->count + count <= head->cap >> 1) {
        while (head->count + count <= head->cap >> 1) {
            head->cap >>= 1;
        }
    }
    // If a resize isn't needed don't do anything
    else {
        return;
    }

    _es_da_header_t *new_head = es_realloc(head, sizeof(_es_da_header_t) + head->cap * head->size);
    if (new_head == NULL) {
        es_free(head);
        *arr = NULL;
        return;
    }

    *arr = _es_da_ptr(new_head);
}

usize_t es_da_count(void *arr) {
    if (arr == NULL) {
        return 0;
    }
    return _es_da_head(arr)->count;
}

/*=========================*/
// Assert
/*=========================*/

void _es_assert_impl(const char *file, u32_t line, const char *expr_str, b8_t expr, const char *fmt, ...) {
    if (expr) {
        return;
    }

    printf("%s:%u: Assertion (%s) failed", file, line, expr_str);
    
    if (fmt) {
        printf(": '");
        va_list ptr;
        va_start(ptr, fmt);
        vprintf(fmt, ptr);
        va_end(ptr);
        printf("'");
    }

    printf("\n");
    abort();
}

/*=========================*/
// Hash table
/*=========================*/

// I just stole this.
usize_t es_hash_str(const char *str) {
    es_assert(sizeof(usize_t) == 8 || sizeof(usize_t) == 4, "Unsupported architecture.", NULL);

    // 64-bit system
    if (sizeof(usize_t) == 8) {
        // Magic number
        usize_t hash = 5381;
        i32_t c;
        while ((c = *str++)) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
    // 32-bit system
    usize_t hash1 = 5381;
    usize_t hash2 = 5381;
    usize_t i = strlen(str);
    while (i--) {
        char c= str[i];
        hash1 = ((hash1 << 5) + hash1) ^ c;
        hash2 = ((hash2 << 5) + hash2) ^ c;
    }
    return (hash1 >> 0) * 4096 + (hash2 >> 0);
}

// This is also stolen. I'm not smart enough to understand this.
usize_t es_siphash(const void *p, usize_t len, usize_t seed) {
    const u8_t *d = p;
    usize_t v0, v1, v2, v3, data;

    v0 = ((((usize_t) 0x736f6d65 << 16) << 16) + 0x70736575) ^  seed;
    v1 = ((((usize_t) 0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
    v2 = ((((usize_t) 0x6c796765 << 16) << 16) + 0x6e657261) ^  seed;
    v3 = ((((usize_t) 0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

    // Rotate left.
    #define es_siprotl(val, n) (((val) << (n) | ((val) >> (sizeof(usize_t) * 8 - (n)))))
    #define es_sipround() do {\
        v0 += v1; v1 = es_siprotl(v1, 13); v1 ^= v0; v0 = es_siprotl(v0, sizeof(usize_t) * 4); \
        v2 += v3; v3 = es_siprotl(v3, 16); v3 ^= v2; \
        v2 += v1; v1 = es_siprotl(v1, 17); v1 ^= v2; v2 = es_siprotl(v2, sizeof(usize_t) * 4); \
        v0 += v3; v3 = es_siprotl(v3, 21); v3 ^= v0; \
    } while (0)

    usize_t i;
    for (i = 0; i+sizeof(usize_t) <= len; i += sizeof(usize_t), d += sizeof(usize_t)) {
        data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        data |= (usize_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16;

        v3 ^= data;
        for (usize_t j = 0; j < ES_SIPHASH_C_ROUNDS; j++) {
            es_sipround();
        }
        v0 ^= data;
    }
    data = len << (sizeof(usize_t) * 7);
    switch (len - i) {
        case 7: data |= ((usize_t) d[6] << 24) << 24; // fallthrough
        case 6: data |= ((usize_t) d[5] << 20) << 20; // fallthrough
        case 5: data |= ((usize_t) d[4] << 16) << 16; // fallthrough
        case 4: data |= (d[3] << 24); // fallthrough
        case 3: data |= (d[2] << 16); // fallthrough
        case 2: data |= (d[1] << 8); // fallthrough
        case 1: data |= d[0]; // fallthrough
        case 0: break;
    }
    v3 ^= data;
    for (i = 0; i < ES_SIPHASH_C_ROUNDS; i++) {
        es_sipround();
    }
    v0 ^= data;
    v2 ^= 0xff;
    for (i = 0; i < ES_SIPHASH_D_ROUNDS; i++) {
        es_sipround();
    }

    return v0^v1^v2^v3;
    #undef es_siprotl
    #undef es_sipround
}

// Search for wanted entry, if not found, find the first dead entry and return that.
usize_t _es_hash_table_get_index(usize_t wanted_hash, void **entries, usize_t entry_count, usize_t entry_size, usize_t hash_offset, usize_t state_offset) {
    u8_t *ptr = *entries;
    usize_t index = wanted_hash % entry_count;
    usize_t entry_hash = *(usize_t *) (ptr + hash_offset);
    // Find wanted entry.
    for (usize_t count = 0; count < entry_count; count++) {
        u8_t *entry_ptr = ptr + index * entry_size;
        entry_hash = *(usize_t *) (entry_ptr + hash_offset);
        _es_hash_table_entry_state_t state = *(_es_hash_table_entry_state_t *) (entry_ptr + state_offset);
        // Return if found.
        if (entry_hash == wanted_hash && state == _ES_HASH_TABLE_ENTRY_ALIVE) {
            return index;
        }
        index = (index + 1) % entry_count;
    }

    // Find first dead entry.
    usize_t dead_index = 0;
    for (usize_t count = 0; count < entry_count; count++) {
        u8_t *entry_ptr = ptr + count * entry_size;
        _es_hash_table_entry_state_t state = *(_es_hash_table_entry_state_t *) (entry_ptr + state_offset);
        if (state == _ES_HASH_TABLE_ENTRY_DEAD) {
            dead_index = count;
            break;
        }
    }

    return dead_index;
}

// Hash value differently dependant on what key type the hash table uses.
usize_t _es_hash_table_hash_key(b8_t is_string, void **ptr, usize_t len) {
    if (is_string) {
        return es_hash_str((const char *) *ptr);
    }
    return es_siphash((void *) ptr, len, ES_HASH_TABLE_SEED);
}

usize_t _es_hash_table_iter_new_impl(void *entries, usize_t state_stride, usize_t entry_size, usize_t cap) {
    for (usize_t i = 0; i < cap; i++) {
        _es_hash_table_entry_state_t state = *(_es_hash_table_entry_state_t *) ((u8_t *) entries + i * entry_size + state_stride);
        if (state == _ES_HASH_TABLE_ENTRY_ALIVE) {
            return i;
        }
    }
    return cap;
}

void _es_hash_table_iter_advance_impl(usize_t state_stride, usize_t entry_size, const void *entries, usize_t *iter, usize_t cap) {
    _es_hash_table_entry_state_t state = _ES_HASH_TABLE_ENTRY_DEAD;
    while (state == _ES_HASH_TABLE_ENTRY_DEAD) {
        (*iter)++;
        if (*iter >= cap) {
            break;
        }
        state = *((_es_hash_table_entry_state_t *) ((u8_t *) entries + *iter * entry_size + state_stride));
    }
}
#endif /*ES_IMPL*/
#endif // ES_H
