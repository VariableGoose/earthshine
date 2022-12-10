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

// Header struct placed before a dynamic array
typedef struct _es_da_header_t {
    // Entry count
    usize_t count;
    // Max entry count
    usize_t cap;
    // Size of entry
    usize_t size;
} _es_da_header_t;

#define _ES_DA_INIT_CAP 8

#define _es_da_head(P) ((_es_da_header_t *) ((u8_t *) (P) - sizeof(_es_da_header_t)))
#define _es_da_ptr(H) ((void *) ((u8_t *) (H) + sizeof(_es_da_header_t)))

ES_API void _es_da_init(void **arr, usize_t size);
ES_API void _es_da_free_impl(void **arr);

ES_API void _es_da_insert_impl(void **arr, const void *data, usize_t index);
ES_API void _es_da_remove_impl(void **arr, usize_t index, void *output);

ES_API void _es_da_insert_fast_impl(void **arr, const void *data, usize_t index);
ES_API void _es_da_remove_fast_impl(void **arr, usize_t index, void *output);

ES_API void _es_da_insert_arr_impl(void **arr, const void *data, usize_t count, usize_t index);
ES_API void _es_da_remove_arr_impl(void **arr, usize_t count, usize_t index, void *output);

ES_API void _es_da_resize(void **arr, isize_t count);

ES_API usize_t es_da_count(void *arr);

#define es_da(T) T *

#define es_da_free(ARR) _es_da_free_impl((void **) &(ARR))

#define es_da_insert(ARR, D, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
#define es_da_remove(ARR, I, OUT) _es_da_remove_impl((void **) &(ARR), I, (OUT))

#define es_da_insert_fast(ARR, D, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_fast_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
#define es_da_remove_fast(ARR, I, OUT) _es_da_remove_fast_impl((void **) &(ARR), (I), (OUT))

#define es_da_push(ARR, D) es_da_insert_fast(ARR, D, es_da_count((ARR)));
#define es_da_pop(ARR, OUT) es_da_remove_fast(ARR, es_da_count((ARR)) - 1, OUT);

#define es_da_insert_arr(ARR, D, C, I) do { \
    _es_da_init((void **) &(ARR), sizeof(*(ARR))); \
    _es_da_insert_arr_impl((void **) &(ARR), (D), (C), (I)); \
} while (0)
#define es_da_remove_arr(ARR, C, I, OUT) _es_da_remove_arr_impl((void **) &(ARR), (C), (I), (OUT))

#define es_da_push_arr(ARR, D, C) es_da_insert_arr(ARR, D, C, es_da_count((ARR)))
#define es_da_pop_arr(ARR, C, OUT) es_da_remove_arr(ARR, C, es_da_count((ARR)) - (C), OUT)

/*=========================*/
// Assert
/*=========================*/

ES_API void _es_assert_impl(const char *file, u32_t line, const char *expr_str, b8_t expr, const char *fmt, ...);

#define es_assert(EXPR, FMT, ...) _es_assert_impl(__FILE__, __LINE__, #EXPR, (EXPR), FMT, ##__VA_ARGS__)

/*=========================*/
// Utils
/*=========================*/

// Get length of an array
#define es_arr_len(ARR) ((sizeof(ARR) / sizeof((ARR)[0])))

// Get stride of a memebr of a struct
#define es_offset(S, M) ((usize_t) &(((S *) 0)->M))

/*=========================*/
// Hash table
/*=========================*/

// Hashing
#ifndef ES_SIPHASH_C_ROUNDS
#define ES_SIPHASH_C_ROUNDS 1
#endif // ES_SIPHASH_C_ROUNDS
#ifndef ES_SIPHASH_D_ROUNDS
#define ES_SIPHASH_D_ROUNDS 1
#endif // ES_SIPHASH_D_ROUNDS

ES_API usize_t es_hash_str(const char *str);
ES_API usize_t es_siphash(const void *data, usize_t len, usize_t seed);

// Data structure
#define ES_HASH_TABLE_MAX_CAP 0.75f
#define ES_HASH_TABLE_SEED 0x2664424Cul

typedef enum _es_hash_table_entry_state_t {
    _ES_HASH_TABLE_ENTRY_DEAD  = 0,
    _ES_HASH_TABLE_ENTRY_ALIVE = 1,
} _es_hash_table_entry_state_t;

#define _es_hash_table_entry(K, V) struct { \
    K key; \
    V value; \
    _es_hash_table_entry_state_t state; \
    usize_t hash; \
}

#define es_hash_table(K, V) struct { \
    es_da(_es_hash_table_entry(K, V)) entries; \
    _es_hash_table_entry(K, V) *temp_entry; \
    K temp_key; \
    b8_t string_key; \
    es_da(usize_t) iter; \
} *

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
} while (0)

#define es_hash_table_string_key(HT) do { \
    _es_hash_table_init(HT); \
    (HT)->string_key = true; \
} while (0)

#define _es_hash_table_insert_entry(ITER, ENTRIES, HASH, CAP, K, V) do { \
    usize_t index = (HASH) % (CAP); \
    usize_t count = 0; \
    usize_t search_hash = (ENTRIES)[index].hash; \
    for (;;) { \
        if (count >= CAP || search_hash == (HASH) || (ENTRIES)[index].state == _ES_HASH_TABLE_ENTRY_DEAD) { \
            break; \
        } \
        index = (index + 1) % (CAP); \
        search_hash = (ENTRIES)[index].hash; \
        count++; \
    } \
    if ((ENTRIES)[index].state == _ES_HASH_TABLE_ENTRY_DEAD) { \
        es_da_push((ITER), index); \
    } \
    (ENTRIES)[index].key   = (K); \
    (ENTRIES)[index].value = (V); \
    (ENTRIES)[index].state = _ES_HASH_TABLE_ENTRY_ALIVE; \
    (ENTRIES)[index].hash  = (HASH); \
} while(0)

#define _es_hash_table_resize(HT) do { \
    _es_hash_table_init(HT); \
    es_da(__typeof__(*((HT)->entries))) new_entries = NULL; \
    es_da(usize_t) new_iter = NULL; \
    usize_t cap = es_da_count((HT)->entries); \
    if (es_da_count((HT)->iter) >= cap * ES_HASH_TABLE_MAX_CAP) { \
        es_da_push_arr(new_entries, NULL, cap * 2); \
    } \
    if (new_entries == NULL) { \
        break; \
    } \
    for (usize_t i = 0; i < es_da_count((HT)->iter); i++) { \
        __typeof__(*((HT)->entries)) curr = (HT)->entries[(HT)->iter[i]]; \
        if (curr.state == _ES_HASH_TABLE_ENTRY_DEAD) { \
            continue; \
        } \
        _es_hash_table_insert_entry(new_iter, new_entries, curr.hash, es_da_count(new_entries), curr.key, curr.value); \
    } \
    es_da_free((HT)->entries); \
    es_da_free((HT)->iter); \
    (HT)->entries = new_entries; \
    (HT)->iter = new_iter; \
} while (0)

#define es_hash_table_insert(HT, K, V) do { \
    _es_hash_table_resize(HT); \
    __typeof__((HT)->entries->key) es_ht_temp_key = (K); \
    usize_t hash = _es_hash_table_hash_key((HT)->string_key, (void **) &es_ht_temp_key, sizeof((HT)->temp_key)); \
    usize_t cap = es_da_count((HT)->entries); \
    _es_hash_table_insert_entry((HT)->iter, (HT)->entries, hash, cap, K, V); \
} while (0)

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

#define es_hash_table_remove(HT, K) do { \
    if ((HT) == NULL) { \
        break; \
    } \
    (HT)->temp_key = (K); \
    __typeof__((HT)->entries) es_ht_temp_entry = &(HT)->entries[_es_hash_table_get_index( \
        _es_hash_table_hash_key((HT)->string_key, (void **) &(HT)->temp_key, sizeof((HT)->temp_key)), \
        (void **) &(HT)->entries, \
        es_da_count((HT)->entries), \
        sizeof(*(HT)->entries), \
        es_offset(__typeof__(*(HT)->entries), hash), \
        es_offset(__typeof__(*(HT)->entries), state) \
    )]; \
    memset(es_ht_temp_entry, 0, sizeof(*(HT)->entries)); \
    (HT)->alive_entries--; \
} while (0)

#define es_hash_table_clear(HT) do { \
    __typeof__((HT)->entries) new_entries = NULL; \
    es_da_push_arr(new_entries, NULL, 8); \
    es_da_free((HT)->entries); \
    (HT)->entries = new_entries; \
    (HT)->alive_entries = 0; \
} while (0)

#define es_hash_table_free(HT) do { \
    if ((HT) != NULL) { \
        es_da_free((HT)->entries); \
        es_da_free((HT)->iter); \
        es_free(HT); \
    } \
} while (0) \

#define es_hash_table_count(HT) es_da_count((HT)->iter)

typedef usize_t es_hash_table_iter_t;
#define es_hash_table_iter_valid(HT, IT) ((HT) == NULL || (HT)->iter == NULL ? false : (IT) < es_da_count((HT)->iter))
#define es_hash_table_iter_advance(HT, IT) ((IT)++)
#define es_hash_table_iter_get(HT, IT) (HT)->entries[(HT)->iter[(IT)]].value
#define es_hash_table_iter_get_key(HT, IT) (HT)->entries[(HT)->iter[(IT)]].key

ES_API usize_t _es_hash_table_get_index(usize_t wanted_hash, void **entries, usize_t entry_count, usize_t entry_size, usize_t hash_offset, usize_t state_offset);
ES_API usize_t _es_hash_table_hash_key(b8_t is_string, void **ptr, usize_t len);

/*=========================*/
// Implementation
/*=========================*/

#endif // ES_H
