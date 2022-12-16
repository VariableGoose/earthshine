/*====================================================================================================
    * Copyright: Linus Erik Pontus Kåreblom
    * Earthshine: A general purpose single header library
    * File: es.h
    * Version: 1.6
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
// Includes
/*=========================*/

#include <string.h>
#include <math.h>

//
// OS Specific
//

// Linux
#ifdef ES_OS_LINUX
#include <pthread.h>
#endif // ES_OS_LINUX

// Windows
#ifdef ES_OS_WIN32
#include <windows.h>
#endif

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

#define es_max(A, B) ((A) > (B) ? (A) : (B))
#define es_min(A, B) ((A) < (B) ? (A) : (B))
#define es_clamp(V, MIN, MAX) ((V) < (MIN) ? (MIN) : (V) > (MAX) ? (MAX) : (V))
#define es_lerp(A, B, T) ((A) + ((B) - (A)) * (T))

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
// Threading
/*=========================*/
typedef void (*es_thread_proc_t)(void *arg);
typedef usize_t es_thread_t;
typedef struct es_mutex_t {
#ifdef ES_OS_LINUX
    pthread_mutex_t handle;
#endif // ES_OS_LINUX
#ifdef ES_OS_WIN32
    HANDLE handle;
#endif // ES_OS_WIN32

} es_mutex_t;

ES_API es_thread_t es_thread(es_thread_proc_t proc, void *arg);
ES_API es_thread_t es_thread_get_self(void);
ES_API void es_thread_wait(es_thread_t thread);

ES_API es_mutex_t es_mutex_init(void);
ES_API void es_mutex_free(es_mutex_t *mutex);
ES_API void es_mutex_lock(es_mutex_t *mutex);
ES_API void es_mutex_unlock(es_mutex_t *mutex);

/*=========================*/
// Filesystem
/*=========================*/

ES_API b8_t es_file_write(const char *filepath, const char *content);
ES_API b8_t es_file_append(const char *filepath, const char *content);
ES_API char *es_file_read(const char *filepath);
ES_API b8_t es_file_exists(const char *filepath);

/*=========================*/
// Math
/*=========================*/

// 2D vector
typedef struct v2_t { f32_t x, y; } v2_t;

ES_INLINE v2_t v2(f32_t x, f32_t y) { return (v2_t) {x, y}; }
ES_INLINE v2_t v2s(f32_t scaler) { return (v2_t) {scaler, scaler}; }

ES_INLINE v2_t v2_mul(v2_t a, v2_t b) { return (v2_t) {a.x * b.x, a.y * b.y}; }
ES_INLINE v2_t v2_div(v2_t a, v2_t b) { return (v2_t) {a.x / b.x, a.y / b.y}; }
ES_INLINE v2_t v2_add(v2_t a, v2_t b) { return (v2_t) {a.x + b.x, a.y + b.y}; }
ES_INLINE v2_t v2_sub(v2_t a, v2_t b) { return (v2_t) {a.x - b.x, a.y - b.y}; }

ES_INLINE v2_t v2_muls(v2_t vec, f32_t s) { return (v2_t) {vec.x * s, vec.y * s}; }
ES_INLINE v2_t v2_divs(v2_t vec, f32_t s) { return (v2_t) {vec.x / s, vec.y / s}; }
ES_INLINE v2_t v2_adds(v2_t vec, f32_t s) { return (v2_t) {vec.x + s, vec.y + s}; }
ES_INLINE v2_t v2_subs(v2_t vec, f32_t s) { return (v2_t) {vec.x - s, vec.y - s}; }

ES_INLINE f32_t v2_mag(v2_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y); }
ES_INLINE v2_t  v2_norm(v2_t vec) { return v2_muls(vec, 1.0f / v2_mag(vec)); }
ES_INLINE f32_t v2_dot(v2_t a, v2_t b) { return a.x*b.x + a.y*b.y; }
ES_INLINE f32_t v2_cross(v2_t a, v2_t b) { return a.x*b.y - a.y*b.x; }

// 3D vector
typedef struct v3_t { f32_t x, y, z; } v3_t;

ES_INLINE v3_t v3(f32_t x, f32_t y, f32_t z) { return (v3_t) {x, y, z}; }
ES_INLINE v3_t v3s(f32_t scaler) { return (v3_t) {scaler, scaler, scaler}; }

ES_INLINE v3_t v3_mul(v3_t a, v3_t b) { return (v3_t) {a.x * b.x, a.y * b.y, a.z * b.z}; }
ES_INLINE v3_t v3_div(v3_t a, v3_t b) { return (v3_t) {a.x / b.x, a.y / b.y, a.z / b.z}; }
ES_INLINE v3_t v3_add(v3_t a, v3_t b) { return (v3_t) {a.x + b.x, a.y + b.y, a.z + b.z}; }
ES_INLINE v3_t v3_sub(v3_t a, v3_t b) { return (v3_t) {a.x - b.x, a.y - b.y, a.z - b.z}; }

ES_INLINE v3_t v3_muls(v3_t vec, f32_t s) { return (v3_t) {vec.x * s, vec.y * s, vec.z * s}; }
ES_INLINE v3_t v3_divs(v3_t vec, f32_t s) { return (v3_t) {vec.x / s, vec.y / s, vec.z / s}; }
ES_INLINE v3_t v3_adds(v3_t vec, f32_t s) { return (v3_t) {vec.x + s, vec.y + s, vec.z + s}; }
ES_INLINE v3_t v3_subs(v3_t vec, f32_t s) { return (v3_t) {vec.x - s, vec.y - s, vec.z - s}; }

ES_INLINE f32_t v3_mag(v3_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z); }
ES_INLINE v3_t  v3_norm(v3_t vec) { return v3_muls(vec, 1.0f / v3_mag(vec)); }
ES_INLINE f32_t v3_dot(v3_t a, v3_t b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
ES_INLINE v3_t v3_cross(v3_t a, v3_t b) {
    return v3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

// 4D vector
typedef struct v4_t { f32_t x, y, z, w; } v4_t;

ES_INLINE v4_t v4(f32_t x, f32_t y, f32_t z, f32_t w) { return (v4_t) {x, y, z, w}; }
ES_INLINE v4_t v4s(f32_t scaler) { return (v4_t) {scaler, scaler, scaler, scaler}; }

ES_INLINE v4_t v4_mul(v4_t a, v4_t b) { return (v4_t) {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
ES_INLINE v4_t v4_div(v4_t a, v4_t b) { return (v4_t) {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }
ES_INLINE v4_t v4_add(v4_t a, v4_t b) { return (v4_t) {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
ES_INLINE v4_t v4_sub(v4_t a, v4_t b) { return (v4_t) {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }

ES_INLINE v4_t v4_muls(v4_t vec, f32_t s) { return (v4_t) {vec.x * s, vec.y * s, vec.z * s, vec.w * s}; }
ES_INLINE v4_t v4_divs(v4_t vec, f32_t s) { return (v4_t) {vec.x / s, vec.y / s, vec.z / s, vec.w / s}; }
ES_INLINE v4_t v4_adds(v4_t vec, f32_t s) { return (v4_t) {vec.x + s, vec.y + s, vec.z + s, vec.w + s}; }
ES_INLINE v4_t v4_subs(v4_t vec, f32_t s) { return (v4_t) {vec.x - s, vec.y - s, vec.z - s, vec.w - s}; }

ES_INLINE f32_t v4_mag(v4_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w); }
ES_INLINE v4_t  v4_norm(v4_t vec) { return v4_muls(vec, 1.0f / v4_mag(vec)); }
ES_INLINE f32_t v4_dot(v4_t a, v4_t b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

// 2x2 matrix
typedef struct m2_t { v2_t i, j; } m2_t;

ES_INLINE m2_t m2v(v2_t i, v2_t j) { return (m2_t) {i, j}; }
ES_INLINE m2_t m2f(f32_t ix, f32_t iy, f32_t jx, f32_t jy) { return (m2_t) {{ix, iy}, {jx, jy}}; }
ES_INLINE m2_t m2_identity(void) { return (m2_t) {{1, 0}, {0, 1}}; }
ES_INLINE m2_t m2_muls(m2_t mat, f32_t s) { return (m2_t) {v2_muls(mat.i, s), v2_muls(mat.j, s)}; }
ES_INLINE v2_t m2_mulv(m2_t mat, v2_t vec) { return v2_add(v2_muls(mat.i, vec.x), v2_muls(mat.j, vec.y)); }
ES_INLINE m2_t m2_mul(m2_t a, m2_t b) { return (m2_t) { m2_mulv(b, a.i), m2_mulv(b, a.j) }; }
ES_INLINE f32_t m2_det(m2_t mat) { return mat.i.x * mat.j.y - mat.j.x * mat.i.y; }
ES_INLINE m2_t m2_inv(m2_t mat) { return m2_muls((m2_t) {{mat.j.y, -mat.i.y}, {-mat.j.x, mat.i.x}}, 1.0f / m2_det(mat)); }

// 3x3 matrix
typedef struct m3_t { v3_t i, j, k; } m3_t;

ES_INLINE m3_t m3v(v3_t i, v3_t j, v3_t k) { return (m3_t) {i, j, k}; }
ES_INLINE m3_t m3f(f32_t ix, f32_t iy, f32_t iz, f32_t jx, f32_t jy, f32_t jz, f32_t kx, f32_t ky, f32_t kz) { return (m3_t) {{ix, iy, iz}, {jx, jy, jz}, {kx, ky, kz}}; }
ES_INLINE m3_t m3_identity(void) { return (m3_t) {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; }
ES_INLINE m3_t m3_muls(m3_t mat, f32_t s) { return (m3_t) {v3_muls(mat.i, s), v3_muls(mat.j, s), v3_muls(mat.k, s)}; }
ES_INLINE v3_t m3_mulv(m3_t mat, v3_t vec) { return v3_add(v3_add(v3_muls(mat.i, vec.x), v3_muls(mat.j, vec.y)), v3_muls(mat.k, vec.z)); }
ES_INLINE m3_t m3_mul(m3_t a, m3_t b) { return (m3_t) { m3_mulv(b, a.i), m3_mulv(b, a.j), m3_mulv(b, a.k) }; }
ES_INLINE f32_t m3_det(m3_t mat) { return mat.i.x * m2_det(m2f(mat.j.y, mat.j.z, mat.k.y, mat.k.z)) - mat.j.x * m2_det(m2f(mat.i.y, mat.i.z, mat.k.y, mat.k.z)) + mat.k.x * m2_det(m2f(mat.i.y, mat.i.z, mat.j.y, mat.j.z)); }
ES_API m3_t m3_inv(m3_t mat);

// 4x4 matrix
typedef struct m4_t { v4_t i, j, k, l; } m4_t;

ES_INLINE m4_t m4v(v4_t i, v4_t j, v4_t k, v4_t l) { return (m4_t) {i, j, k, l}; }
ES_INLINE m4_t m4f(f32_t ix, f32_t iy, f32_t iz, f32_t iw, f32_t jx, f32_t jy, f32_t jz, f32_t jw, f32_t kx, f32_t ky, f32_t kz, f32_t kw, f32_t lx, f32_t ly, f32_t lz, f32_t lw) { return (m4_t) {{ix, iy, iz, iw}, {jx, jy, jz, jw}, {kx, ky, kz, kw}, {lx, ly, lz, lw}}; }
ES_INLINE m4_t m4_identity(void) { return (m4_t) {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}; }
ES_INLINE m4_t m4_muls(m4_t mat, f32_t s) { return (m4_t) {v4_muls(mat.i, s), v4_muls(mat.j, s), v4_muls(mat.k, s), v4_muls(mat.l, s)}; }
ES_INLINE v4_t m4_mulv(m4_t mat, v4_t vec) { return v4_add(v4_add(v4_add(v4_muls(mat.i, vec.x), v4_muls(mat.j, vec.y)), v4_muls(mat.k, vec.z)), v4_muls(mat.l, vec.w)); }
ES_INLINE m4_t m4_mul(m4_t a, m4_t b) { return (m4_t) { m4_mulv(b, a.i), m4_mulv(b, a.j), m4_mulv(b, a.k), m4_mulv(b, a.l) }; }
ES_API m4_t m4_inv(m4_t mat);

/*=========================*/
// Implementation
/*=========================*/

#endif // ES_H
