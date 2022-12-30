/*====================================================================================================
    * Copyright: Linus Erik Pontus Kåreblom
    * Earthshine: A general purpose single header library
    * File: es.h
    * Version: 1.12
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

#define _POSIX_C_SOURCE 199309L

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef ES_VULKAN
// Define what surface KHR to use.
#ifdef ES_OS_LINUX
#define VK_USE_PLATFORM_XLIB_KHR
#endif // ES_OS_LINUX
#ifdef ES_OS_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif // ES_OS_WIN32
#include <vulkan/vulkan.h>
#endif // ES_VULKAN

//
// OS Specific
//

// Linux
#ifdef ES_OS_LINUX
#include <pthread.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>
#include <dlfcn.h>
#endif // ES_OS_LINUX

// Windows
#ifdef ES_OS_WIN32
#include <windows.h>
#include <windowsx.h> // Input parsing.
// #pragma comment(lib, "user32.lib")
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
ES_API usize_t es_da_count(const void *arr);

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
#define es_da_push(ARR, D) es_da_insert_fast(ARR, D, es_da_count((ARR)))
// Remove entry at the end of dynamic array.
#define es_da_pop(ARR, OUT) es_da_remove_fast(ARR, es_da_count((ARR)) - 1, OUT)

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

// Retrieve last item in array.
#define es_da_last(ARR) (ARR)[es_da_count(ARR) - 1]

// Iterate over a dynamic array.
#define es_da_iter(DA, I) for (usize_t I = 0; I < es_da_count(DA); i++)

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

#define _es_macro_concat(A, B) A ## B
#define es_macro_concat(A, B) _es_macro_concat(A, B)
#define es_macro_var(NAME) es_macro_concat(_es_macro_variable, es_macro_concat(NAME, __LINE__))

#define ES_I8_MIN  -128
#define ES_I8_MAX   127
#define ES_I16_MIN -32768
#define ES_I16_MAX  32767
#define ES_I32_MIN -2147483648
#define ES_I32_MAX  2147483647
#define ES_I64_MIN  9223372036854775807
#define ES_I64_MAX -9223372036854775808

#define ES_U8_MAX  255
#define ES_U16_MAX 65535
#define ES_U32_MAX 4294967295
#define ES_U64_MAX 18446744073709551615

// Hash string.
ES_API usize_t es_hash_str(const char *str);
// Hash any data (except string).
ES_API usize_t es_siphash(const void *data, usize_t len, usize_t seed);

// Get current time in milliseconds.
ES_API f64_t es_get_time(void);
// Make system sleep for ms milliseconds.
ES_API void es_sleep(usize_t ms);
// Private clock setup function for windows.
ES_API void _es_win32_clock_setup(void);

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
    (HT)->count = 0; \
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
// Strings
/*=========================*/

#define _es_str_head(P) ((_es_str_head_t *) ((u8_t *) P - sizeof(_es_str_head_t)))
#define _es_str_ptr(H) ((es_str_t) ((u8_t *) H + sizeof(_es_str_head_t)))

typedef struct {
    usize_t len;
    b8_t valid;
} _es_str_head_t;
typedef char *es_str_t;

ES_API es_str_t es_strn(const char *str, usize_t len);
ES_API es_str_t es_str(const char *str);
ES_API es_str_t es_str_empty(void);
ES_API es_str_t es_str_reserve(usize_t len);

ES_API es_str_t es_str_u64(u64_t value);
ES_API es_str_t es_str_u32(u32_t value);
ES_API es_str_t es_str_u16(u16_t value);
ES_API es_str_t es_str_u8(u8_t value);

ES_API es_str_t es_str_i64(i64_t value);
ES_API es_str_t es_str_i32(i32_t value);
ES_API es_str_t es_str_i16(i16_t value);
ES_API es_str_t es_str_i8(i8_t value);

ES_API es_str_t es_str_b8(b8_t value);

ES_API void es_str_free(es_str_t *str);
ES_API void es_str_free_list(es_da(es_str_t) *list);

ES_API b8_t es_str_valid(const es_str_t str);
ES_API usize_t es_str_len(const es_str_t str);

ES_API void es_str_concat_len(es_str_t *str, const char *end, usize_t len);
ES_API void es_str_concat(es_str_t *str, const char *end);
ES_API void es_str_concat_char(es_str_t *str, char c);

ES_API void es_str_reverse(es_str_t *str);

ES_API es_str_t es_str_sub_start(const char *str, usize_t len);
ES_API es_str_t es_str_sub_end(const char *str, usize_t len);
ES_API es_str_t es_str_sub_index(const char *str, usize_t start, usize_t end);
ES_API es_str_t es_str_sub_len(const char *str, usize_t start, usize_t len);

ES_API es_da(es_str_t) es_str_split_by_delim(const char *str, char delim);

ES_API i32_t es_str_cmp(es_str_t str, const char *b);

ES_API void _es_str_resize(es_str_t *str, usize_t len);

ES_API usize_t es_cstr_len(const char *str);
ES_API i32_t es_cstr_cmp_len(const char *a, const char *b, usize_t len);
ES_API i32_t es_cstr_cmp(const char *a, const char *b);

ES_API b8_t es_is_whitespace(char c);
ES_API b8_t es_is_alpha(char c);
ES_API b8_t es_is_digit(char c);

/*=========================*/
// Filesystem
/*=========================*/

ES_API b8_t es_file_write(const char *filepath, const char *content);
ES_API b8_t es_file_append(const char *filepath, const char *content);
ES_API es_str_t es_file_read(const char *filepath);
ES_API b8_t es_file_exists(const char *filepath);

/*=========================*/
// Math
/*=========================*/

ES_API f32_t es_float_round(f32_t value, i32_t n);

// 2D vector
typedef struct vec2_t { f32_t x, y; } vec2_t;

ES_INLINE vec2_t vec2(f32_t x, f32_t y) { return (vec2_t) {x, y}; }
ES_INLINE vec2_t vec2s(f32_t scaler) { return (vec2_t) {scaler, scaler}; }

ES_INLINE vec2_t vec2_mul(vec2_t a, vec2_t b) { return (vec2_t) {a.x * b.x, a.y * b.y}; }
ES_INLINE vec2_t vec2_div(vec2_t a, vec2_t b) { return (vec2_t) {a.x / b.x, a.y / b.y}; }
ES_INLINE vec2_t vec2_add(vec2_t a, vec2_t b) { return (vec2_t) {a.x + b.x, a.y + b.y}; }
ES_INLINE vec2_t vec2_sub(vec2_t a, vec2_t b) { return (vec2_t) {a.x - b.x, a.y - b.y}; }

ES_INLINE vec2_t vec2_muls(vec2_t vec, f32_t s) { return (vec2_t) {vec.x * s, vec.y * s}; }
ES_INLINE vec2_t vec2_divs(vec2_t vec, f32_t s) { return (vec2_t) {vec.x / s, vec.y / s}; }
ES_INLINE vec2_t vec2_adds(vec2_t vec, f32_t s) { return (vec2_t) {vec.x + s, vec.y + s}; }
ES_INLINE vec2_t vec2_subs(vec2_t vec, f32_t s) { return (vec2_t) {vec.x - s, vec.y - s}; }

ES_INLINE f32_t  vec2_magnitude(vec2_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y); }
ES_INLINE vec2_t vec2_normalize(vec2_t vec) { return vec2_muls(vec, 1.0f / vec2_magnitude(vec)); }
ES_INLINE f32_t  vec2_dot(vec2_t a, vec2_t b) { return a.x*b.x + a.y*b.y; }
ES_INLINE f32_t  vec2_cross(vec2_t a, vec2_t b) { return a.x*b.y - a.y*b.x; }

ES_INLINE b8_t vec2_equal(vec2_t a, vec2_t b) { return a.x == b.x && a.y == b.y; }
ES_INLINE b8_t vec2_equals(vec2_t a, f32_t s) { return a.x == s   && a.y == s;   }

// 3D vector
typedef struct vec3_t { f32_t x, y, z; } vec3_t;

ES_INLINE vec3_t vec3(f32_t x, f32_t y, f32_t z) { return (vec3_t) {x, y, z}; }
ES_INLINE vec3_t vec3s(f32_t scaler) { return (vec3_t) {scaler, scaler, scaler}; }

ES_INLINE vec3_t vec3_mul(vec3_t a, vec3_t b) { return (vec3_t) {a.x * b.x, a.y * b.y, a.z * b.z}; }
ES_INLINE vec3_t vec3_div(vec3_t a, vec3_t b) { return (vec3_t) {a.x / b.x, a.y / b.y, a.z / b.z}; }
ES_INLINE vec3_t vec3_add(vec3_t a, vec3_t b) { return (vec3_t) {a.x + b.x, a.y + b.y, a.z + b.z}; }
ES_INLINE vec3_t vec3_sub(vec3_t a, vec3_t b) { return (vec3_t) {a.x - b.x, a.y - b.y, a.z - b.z}; }

ES_INLINE vec3_t vec3_muls(vec3_t vec, f32_t s) { return (vec3_t) {vec.x * s, vec.y * s, vec.z * s}; }
ES_INLINE vec3_t vec3_divs(vec3_t vec, f32_t s) { return (vec3_t) {vec.x / s, vec.y / s, vec.z / s}; }
ES_INLINE vec3_t vec3_adds(vec3_t vec, f32_t s) { return (vec3_t) {vec.x + s, vec.y + s, vec.z + s}; }
ES_INLINE vec3_t vec3_subs(vec3_t vec, f32_t s) { return (vec3_t) {vec.x - s, vec.y - s, vec.z - s}; }

ES_INLINE f32_t  vec3_magnitude(vec3_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z); }
ES_INLINE vec3_t vec3_normalize(vec3_t vec) { return vec3_muls(vec, 1.0f / vec3_magnitude(vec)); }
ES_INLINE f32_t  vec3_dot(vec3_t a, vec3_t b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
ES_INLINE vec3_t vec3_cross(vec3_t a, vec3_t b) {
    return vec3(
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    );
}

ES_INLINE b8_t vec3_equal(vec3_t a, vec3_t b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
ES_INLINE b8_t vec3_equals(vec3_t a, f32_t s) { return a.x == s   && a.y == s   && a.z == s;   }

// 4D vector
typedef struct vec4_t { f32_t x, y, z, w; } vec4_t;

ES_INLINE vec4_t vec4(f32_t x, f32_t y, f32_t z, f32_t w) { return (vec4_t) {x, y, z, w}; }
ES_INLINE vec4_t vec4s(f32_t scaler) { return (vec4_t) {scaler, scaler, scaler, scaler}; }

ES_INLINE vec4_t vec4_mul(vec4_t a, vec4_t b) { return (vec4_t) {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w}; }
ES_INLINE vec4_t vec4_div(vec4_t a, vec4_t b) { return (vec4_t) {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w}; }
ES_INLINE vec4_t vec4_add(vec4_t a, vec4_t b) { return (vec4_t) {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; }
ES_INLINE vec4_t vec4_sub(vec4_t a, vec4_t b) { return (vec4_t) {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; }

ES_INLINE vec4_t vec4_muls(vec4_t vec, f32_t s) { return (vec4_t) {vec.x * s, vec.y * s, vec.z * s, vec.w * s}; }
ES_INLINE vec4_t vec4_divs(vec4_t vec, f32_t s) { return (vec4_t) {vec.x / s, vec.y / s, vec.z / s, vec.w / s}; }
ES_INLINE vec4_t vec4_adds(vec4_t vec, f32_t s) { return (vec4_t) {vec.x + s, vec.y + s, vec.z + s, vec.w + s}; }
ES_INLINE vec4_t vec4_subs(vec4_t vec, f32_t s) { return (vec4_t) {vec.x - s, vec.y - s, vec.z - s, vec.w - s}; }

ES_INLINE f32_t  vec4_magnitude(vec4_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w); }
ES_INLINE vec4_t vec4_normalize(vec4_t vec) { return vec4_muls(vec, 1.0f / vec4_magnitude(vec)); }
ES_INLINE f32_t  vec4_dot(vec4_t a, vec4_t b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

ES_INLINE b8_t vec4_equal(vec4_t a, vec4_t b) { return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w; }
ES_INLINE b8_t vec4_equals(vec4_t a, f32_t s) { return a.x == s   && a.y == s   && a.z == s   && a.w == s;   }

// 2x2 matrix
typedef struct mat2_t { vec2_t i, j; } mat2_t;

ES_INLINE mat2_t mat2v(vec2_t i, vec2_t j) { return (mat2_t) {i, j}; }
ES_INLINE mat2_t mat2f(f32_t ix, f32_t iy, f32_t jx, f32_t jy) { return (mat2_t) {{ix, iy}, {jx, jy}}; }
ES_INLINE mat2_t mat2_identity(void) { return (mat2_t) {{1, 0}, {0, 1}}; }
ES_INLINE mat2_t mat2_muls(mat2_t mat, f32_t s) { return (mat2_t) {vec2_muls(mat.i, s), vec2_muls(mat.j, s)}; }
ES_INLINE vec2_t mat2_mulv(mat2_t mat, vec2_t vec) { return vec2_add(vec2_muls(mat.i, vec.x), vec2_muls(mat.j, vec.y)); }
ES_INLINE mat2_t mat2_mul(mat2_t a, mat2_t b) { return (mat2_t) { mat2_mulv(b, a.i), mat2_mulv(b, a.j) }; }
ES_INLINE f32_t mat2_determinate(mat2_t mat) { return mat.i.x * mat.j.y - mat.j.x * mat.i.y; }
ES_INLINE mat2_t mat2_inverse(mat2_t mat) { return mat2_muls((mat2_t) {{mat.j.y, -mat.i.y}, {-mat.j.x, mat.i.x}}, 1.0f / mat2_determinate(mat)); }

ES_INLINE b8_t mat2_equal(mat2_t a, mat2_t b) { return vec2_equal(a.i, b.i) && vec2_equal(a.j, b.j); }

// 3x3 matrix
typedef struct mat3_t { vec3_t i, j, k; } mat3_t;

ES_INLINE mat3_t mat3v(vec3_t i, vec3_t j, vec3_t k) { return (mat3_t) {i, j, k}; }
ES_INLINE mat3_t mat3f(f32_t ix, f32_t iy, f32_t iz, f32_t jx, f32_t jy, f32_t jz, f32_t kx, f32_t ky, f32_t kz) { return (mat3_t) {{ix, iy, iz}, {jx, jy, jz}, {kx, ky, kz}}; }
ES_INLINE mat3_t mat3_identity(void) { return (mat3_t) {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}; }
ES_INLINE mat3_t mat3_muls(mat3_t mat, f32_t s) { return (mat3_t) {vec3_muls(mat.i, s), vec3_muls(mat.j, s), vec3_muls(mat.k, s)}; }
ES_INLINE vec3_t mat3_mulv(mat3_t mat, vec3_t vec) { return vec3_add(vec3_add(vec3_muls(mat.i, vec.x), vec3_muls(mat.j, vec.y)), vec3_muls(mat.k, vec.z)); }
ES_INLINE mat3_t mat3_mul(mat3_t a, mat3_t b) { return (mat3_t) { mat3_mulv(b, a.i), mat3_mulv(b, a.j), mat3_mulv(b, a.k) }; }
ES_INLINE f32_t mat3_determinate(mat3_t mat) { return mat.i.x * mat2_determinate(mat2f(mat.j.y, mat.j.z, mat.k.y, mat.k.z)) - mat.j.x * mat2_determinate(mat2f(mat.i.y, mat.i.z, mat.k.y, mat.k.z)) + mat.k.x * mat2_determinate(mat2f(mat.i.y, mat.i.z, mat.j.y, mat.j.z)); }
ES_API mat3_t mat3_inverse(mat3_t mat);

ES_INLINE b8_t mat3_equal(mat3_t a, mat3_t b) { return vec3_equal(a.i, b.i) && vec3_equal(a.j, b.j) && vec3_equal(a.k, b.k); }

// 4x4 matrix
typedef struct mat4_t { vec4_t i, j, k, l; } mat4_t;

ES_INLINE mat4_t mat4v(vec4_t i, vec4_t j, vec4_t k, vec4_t l) { return (mat4_t) {i, j, k, l}; }
ES_INLINE mat4_t mat4f(f32_t ix, f32_t iy, f32_t iz, f32_t iw, f32_t jx, f32_t jy, f32_t jz, f32_t jw, f32_t kx, f32_t ky, f32_t kz, f32_t kw, f32_t lx, f32_t ly, f32_t lz, f32_t lw) { return (mat4_t) {{ix, iy, iz, iw}, {jx, jy, jz, jw}, {kx, ky, kz, kw}, {lx, ly, lz, lw}}; }
ES_INLINE mat4_t mat4_identity(void) { return (mat4_t) {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}; }
ES_INLINE mat4_t mat4_muls(mat4_t mat, f32_t s) { return (mat4_t) {vec4_muls(mat.i, s), vec4_muls(mat.j, s), vec4_muls(mat.k, s), vec4_muls(mat.l, s)}; }
ES_INLINE vec4_t mat4_mulv(mat4_t mat, vec4_t vec) { return vec4_add(vec4_add(vec4_add(vec4_muls(mat.i, vec.x), vec4_muls(mat.j, vec.y)), vec4_muls(mat.k, vec.z)), vec4_muls(mat.l, vec.w)); }
ES_INLINE mat4_t mat4_mul(mat4_t a, mat4_t b) { return (mat4_t) { mat4_mulv(b, a.i), mat4_mulv(b, a.j), mat4_mulv(b, a.k), mat4_mulv(b, a.l) }; }
ES_API mat4_t mat4_inverse(mat4_t mat);

ES_INLINE b8_t matw_equal(mat4_t a, mat4_t b) { return vec4_equal(a.i, b.i) && vec4_equal(a.j, b.j) && vec4_equal(a.k, b.k) && vec4_equal(a.l, b.l); }

/*=========================*/
// Profiler
/*=========================*/

typedef struct _es_profile_entry_t {
    es_da(struct _es_profile_entry_t) children;
    struct _es_profile_entry_t *parent;
    const char *name;
    f64_t t0;
    f64_t time;
    u32_t runs; 
} _es_profile_t;

ES_GLOBAL _es_profile_t _es_root_profile;
ES_GLOBAL _es_profile_t *_es_curr_profile;

ES_API _es_profile_t _es_profile_new(const char *name);
ES_API void _es_profile_begin(const char *name);
ES_API void _es_profile_end(void);
ES_API void _es_profile_print(const _es_profile_t *prof, usize_t gen);
ES_API void es_profile_print(void);

#define es_profile(NAME) for (b8_t es_macro_var(i) = ((void) _es_profile_begin(NAME), false); !es_macro_var(i); es_macro_var(i) = true, (void) _es_profile_end())

/*=========================*/
// Windowing
/*=========================*/

typedef enum es_key_t {
    ES_KEY_NULL = -1,

    // Letters
    ES_KEY_A,
    ES_KEY_B,
    ES_KEY_C,
    ES_KEY_D,
    ES_KEY_E,
    ES_KEY_F,
    ES_KEY_G,
    ES_KEY_H,
    ES_KEY_I,
    ES_KEY_J,
    ES_KEY_K,
    ES_KEY_L,
    ES_KEY_M,
    ES_KEY_N,
    ES_KEY_O,
    ES_KEY_P,
    ES_KEY_Q,
    ES_KEY_R,
    ES_KEY_S,
    ES_KEY_T,
    ES_KEY_U,
    ES_KEY_V,
    ES_KEY_W,
    ES_KEY_X,
    ES_KEY_Y,
    ES_KEY_Z,

    // Numbers
    ES_KEY_0,
    ES_KEY_1,
    ES_KEY_2,
    ES_KEY_3,
    ES_KEY_4,
    ES_KEY_5,
    ES_KEY_6,
    ES_KEY_7,
    ES_KEY_8,
    ES_KEY_9,

    // Function keys
    ES_KEY_F1,
    ES_KEY_F2,
    ES_KEY_F3,
    ES_KEY_F4,
    ES_KEY_F5,
    ES_KEY_F6,
    ES_KEY_F7,
    ES_KEY_F8,
    ES_KEY_F9,
    ES_KEY_F10,
    ES_KEY_F11,
    ES_KEY_F12,
    ES_KEY_F13,
    ES_KEY_F14,
    ES_KEY_F15,
    ES_KEY_F16,
    ES_KEY_F17,
    ES_KEY_F18,
    ES_KEY_F19,
    ES_KEY_F20,
    ES_KEY_F21,
    ES_KEY_F22,
    ES_KEY_F23,
    ES_KEY_F24,

    ES_KEY_ESC,
    ES_KEY_TAB,
    ES_KEY_ENTER,
    ES_KEY_SPACE,
    ES_KEY_BACKSPACE,
    ES_KEY_CAPS_LOCK,

    // Arrows.
    ES_KEY_LEFT,
    ES_KEY_DOWN,
    ES_KEY_UP,
    ES_KEY_RIGHT,

    // Mod keys.
    ES_KEY_SHIFT_L,
    ES_KEY_SHIFT_R,
    ES_KEY_CTRL_L,
    ES_KEY_CTRL_R,
    ES_KEY_ALT_L,
    ES_KEY_ALT_R,
    ES_KEY_SUPER_L,
    ES_KEY_SUPER_R,

    // Symbols.
    ES_KEY_SEMICOLON,  // ;
    ES_KEY_APOSTROPHE, // '
    ES_KEY_EQUAL,      // =/+
    ES_KEY_COMMA,      // ,
    ES_KEY_MINUS,      // -
    ES_KEY_PERIOD,     // .
    ES_KEY_SLASH,      // /
    ES_KEY_GRAVE,      // `
    ES_KEY_BRACKET_L,  // [
    ES_KEY_PIPE,       // |
    ES_KEY_BRACKET_R,  // ]

    // Duplicates.
    ES_KEY_QUOTE      = ES_KEY_APOSTROPHE,
    ES_KEY_PLUS       = ES_KEY_EQUAL,
    ES_KEY_BACKSLASH  = ES_KEY_PIPE,

    ES_KEY_COUNT = ES_KEY_BRACKET_R + 1,
} es_key_t;

typedef enum es_button_t {
    ES_BUTTON_LEFT,
    ES_BUTTON_MIDDLE,
    ES_BUTTON_RIGHT,

    ES_BUTTON_COUNT,
} es_button_t;

typedef enum es_key_action_t {
    ES_KEY_ACTION_RELEASE,
    ES_KEY_ACTION_PRESS,
    ES_KEY_ACTION_REPEAT,
} es_key_action_t;

// Window handle.
typedef void es_window_t;

// Window callbacks.
typedef void (*es_window_resize_callback_t)(es_window_t *window, i32_t width, i32_t height);
typedef void (*es_window_key_callback_t)(es_window_t *window, es_key_t keycode, usize_t mod, es_key_action_t action);
typedef void (*es_window_mouse_button_callback_t)(es_window_t *window, es_button_t button, es_key_action_t action);
typedef void (*es_window_cursor_position_callback)(es_window_t *window, i32_t x, i32_t y);
typedef void (*es_window_scroll_callback)(es_window_t *window, i32_t offset);

// Private window struct.
typedef struct _es_window_t {
#ifdef ES_OS_LINUX
    Display *display;
    Window window;
    Atom wm_delete_window;
    XIC input_context;
    XKeyEvent prev_key_event;
#endif // ES_OS_LINUX
#ifdef ES_OS_WIN32
    HINSTANCE instance;
    HWND window;
#endif // ES_OS_WIN32
    b8_t is_open;
    vec2_t size;

    es_window_resize_callback_t resize_callback;
    es_window_key_callback_t key_callback;
    es_window_mouse_button_callback_t mouse_button_callback;
    es_window_cursor_position_callback cursor_position_callback;
    es_window_scroll_callback scroll_callback;
} _es_window_t;

// Create and initialize a window.
ES_API es_window_t *es_window_init(i32_t width, i32_t height, const char *title, b8_t resizable);
// Free everything associated with window.
ES_API void es_window_free(es_window_t *window);
// Poll for window events.
ES_API void es_window_poll_events(es_window_t *window);
// Check if window is open.
ES_API b8_t es_window_is_open(es_window_t *window);
// Retrieve window size.
ES_API vec2_t es_window_get_size(es_window_t *window);
// Provide a resize callback to be called when window is resized.
ES_API void es_window_set_resize_callback(es_window_t *window, es_window_resize_callback_t callback);
// Provide a key callback to be called when a keyboard key is pressed or released.
ES_API void es_window_set_key_callback(es_window_t *window, es_window_key_callback_t callback);
// Provide a mouse button callback to be called when a mouse button is pressed or released.
ES_API void es_window_set_mouse_button_callback(es_window_t *window, es_window_mouse_button_callback_t callback);
// Provide a cursor position callback to be called when the cursor position changes.
ES_API void es_window_set_cursor_position_callback(es_window_t *window, es_window_cursor_position_callback callback);
// Provide a scroll callback to be called when a scroll event happens.
ES_API void es_window_set_scroll_callback(es_window_t *window, es_window_scroll_callback callback);
#ifdef ES_VULKAN
// Create a vulkan surface for window.
ES_API VkSurfaceKHR es_window_vulkan_surface(const es_window_t *window, VkInstance instance);
#endif // ES_VULKAN
#ifdef ES_OS_LINUX
// Internal linus function for translating XLib keysyms to es_key_t.
ES_API es_key_t _es_window_translate_keysym(KeySym keysym);
#endif // ES_OS_LINUX
#ifdef ES_OS_WIN32
// Internal win32 function for processing window events.
ES_API LRESULT CALLBACK _es_window_process_message(HWND hwnd, u32_t msg, WPARAM w_param, LPARAM l_param);
// Internal win32 function for translating keypresses to es_key_t.
ES_API es_key_t _es_window_translate_scancode(u16_t scancode);
#endif // ES_OS_WIN32

/*=========================*/
// Library loading
/*=========================*/

// Library handle.
typedef struct es_lib_t { void *handle; b8_t valid; } es_lib_t;
// Library function.
typedef struct es_lib_func_t { void (*func)(void); b8_t valid; } es_lib_func_t;

// Dynamically load a library.
ES_API es_lib_t es_library_init(const char *filepath);
// Dynamically load a function from a library.
ES_API es_lib_func_t es_library_function(es_lib_t lib, const char *name);
// Close dynamically loaded library.
ES_API void es_library_free(es_lib_t *lib);

/*=========================*/
// Unit testing
/*=========================*/

typedef struct {
    b8_t success;
    u32_t line;
    es_str_t file;
    const char *func;
} es_unit_result_t;

typedef es_unit_result_t (*es_unit_func_t)(const char *);

typedef struct es_unit_test_t {
    es_da(es_unit_result_t) success;
    es_da(es_unit_result_t) fail;
    es_da(es_unit_result_t) result;
    es_da(es_str_t) not_found;
    b8_t valid;
} es_unit_test_t;

#define es_unit(N) extern es_unit_result_t es_unit_test_##N(const char *func_name); es_unit_result_t es_unit_test_##N(const char *func_name)
#define es_unit_fail() return (es_unit_result_t) { false, __LINE__, es_str(__FILE__), func_name };
#define es_unit_success() return (es_unit_result_t) { true, __LINE__, es_str(__FILE__), func_name };
#define es_unit_check(EXPR) do { if (EXPR) { es_unit_success() } else { es_unit_fail(); } } while(0)

ES_API es_unit_test_t es_unit_test(es_da(const char *) source_files, const char *library_file);
ES_API void es_unit_test_free(es_unit_test_t *test);

/*=========================*/
// Logging
/*=========================*/

typedef es_str_t (*es_format_expander_t)(es_da(es_str_t), va_list);
typedef struct _es_formatter_t {
    es_hash_table(const char *, es_format_expander_t) formats;
    b8_t initialized;
} _es_formatter_t;

extern _es_formatter_t _es_formatter_g;

ES_API void es_formatter_init(void);
ES_API void es_formatter_free(void);
ES_API void es_formatter_add_format(const char *trigger, es_format_expander_t expander);

ES_API es_str_t _es_format(const char *fmt, va_list va_ptr);
ES_API es_str_t es_format(const char *fmt, ...);
ES_API void es_log(const char *fmt, ...);

// Expanders.
ES_API es_str_t es_format_expand(const char *fmt, ...);

// Base types.
ES_API es_str_t _es_format_expander_u64(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_i64(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_f64(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_b8(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_str(es_da(es_str_t) args, va_list va_ptr);
// ES types.
ES_API es_str_t _es_format_expander_vec2(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_vec3(es_da(es_str_t) args, va_list va_ptr);
ES_API es_str_t _es_format_expander_vec4(es_da(es_str_t) args, va_list va_ptr);

/*=========================*/
// Error handler
/*=========================*/

typedef enum es_error_severity_t {
    ES_ERROR_SEVERITY_FATAL   = 1 << 0,
    ES_ERROR_SEVERITY_ERROR   = 1 << 1,
    ES_ERROR_SEVERITY_WARNING = 1 << 2,
    ES_ERROR_SEVERITY_INFO    = 1 << 3,
    ES_ERROR_SEVERITY_DEBUG   = 1 << 4,
} es_error_severity_t;

typedef struct es_error_t {
    const char *file;
    i32_t line;
    const char *message;
    es_error_severity_t severity;
} es_error_t;

// Error callback type.
typedef void (*es_error_callback_t)(es_error_t);

ES_GLOBAL es_error_t _error_stack_g[32];
ES_GLOBAL u32_t _es_error_stack_i;
ES_GLOBAL es_error_callback_t _es_error_callback_g;
ES_GLOBAL const es_error_t ES_NULL_ERROR;
ES_GLOBAL es_error_severity_t _es_error_severity_filter;

// Internal error reporting function.
ES_API void _es_error_report(const char *file, i32_t line, const char *message, es_error_severity_t severity);
// Retrieve an error from the stack. If none exists ES_NULL_ERROR will be returned.
ES_API es_error_t es_error_get(void);
// Specify an error callback.
ES_API void es_error_set_callback(es_error_callback_t callback);
// Specify which error severities are important.
ES_API void es_error_set_filter(es_error_severity_t filter);
// Check if an error is NULL.
ES_API b8_t es_error_is_null(es_error_t error);

// Report an error.
#define es_error(MSG, SEVERITY) _es_error_report(__FILE__, __LINE__, MSG, SEVERITY)

/*=========================*/
// Implementation
/*=========================*/

#endif // ES_H
