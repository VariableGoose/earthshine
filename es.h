/*====================================================================================================
    * Copyright: Linus Erik Pontus Kåreblom
    * Earthshine: A general purpose single header library
    * File: es.h
    * Version: 1.10
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

ES_API void es_str_free(es_str_t *str);

ES_API b8_t es_str_valid(const es_str_t str);
ES_API usize_t es_str_len(const es_str_t str);

ES_API es_str_t es_str_concat_len(es_str_t str, const char *end, usize_t len);
ES_API es_str_t es_str_concat(es_str_t str, const char *end);

ES_API es_str_t es_str_sub_start(es_str_t str, usize_t len);
ES_API es_str_t es_str_sub_end(es_str_t str, usize_t len);
ES_API es_str_t es_str_sub_index(es_str_t str, usize_t start, usize_t end);
ES_API es_str_t es_str_sub_len(es_str_t str, usize_t start, usize_t len);

ES_API es_str_t _es_str_resize(es_str_t str, usize_t len);

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

ES_INLINE f32_t vec4_magnitude(vec4_t vec) { return sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z + vec.w*vec.w); }
ES_INLINE vec4_t  vec4_normalize(vec4_t vec) { return vec4_muls(vec, 1.0f / vec4_magnitude(vec)); }
ES_INLINE f32_t vec4_dot(vec4_t a, vec4_t b) { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }

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

// 4x4 matrix
typedef struct mat4_t { vec4_t i, j, k, l; } mat4_t;

ES_INLINE mat4_t mat4v(vec4_t i, vec4_t j, vec4_t k, vec4_t l) { return (mat4_t) {i, j, k, l}; }
ES_INLINE mat4_t mat4f(f32_t ix, f32_t iy, f32_t iz, f32_t iw, f32_t jx, f32_t jy, f32_t jz, f32_t jw, f32_t kx, f32_t ky, f32_t kz, f32_t kw, f32_t lx, f32_t ly, f32_t lz, f32_t lw) { return (mat4_t) {{ix, iy, iz, iw}, {jx, jy, jz, jw}, {kx, ky, kz, kw}, {lx, ly, lz, lw}}; }
ES_INLINE mat4_t mat4_identity(void) { return (mat4_t) {{1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}}; }
ES_INLINE mat4_t mat4_muls(mat4_t mat, f32_t s) { return (mat4_t) {vec4_muls(mat.i, s), vec4_muls(mat.j, s), vec4_muls(mat.k, s), vec4_muls(mat.l, s)}; }
ES_INLINE vec4_t mat4_mulv(mat4_t mat, vec4_t vec) { return vec4_add(vec4_add(vec4_add(vec4_muls(mat.i, vec.x), vec4_muls(mat.j, vec.y)), vec4_muls(mat.k, vec.z)), vec4_muls(mat.l, vec.w)); }
ES_INLINE mat4_t mat4_mul(mat4_t a, mat4_t b) { return (mat4_t) { mat4_mulv(b, a.i), mat4_mulv(b, a.j), mat4_mulv(b, a.k), mat4_mulv(b, a.l) }; }
ES_API mat4_t mat4_inverse(mat4_t mat);

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
// Memory
/*=========================*/

ES_API void es_memcpy(void *dest, const void *src, usize_t len);
ES_API void es_memset(void *dest, i32_t c, usize_t len);
ES_API i32_t es_memcmp(const void *a, const void *b, usize_t len);

/*=========================*/
// Unit testing
/*=========================*/

typedef struct {
    b8_t success;
    u32_t line;
    const char *file;
    const char *func;
} es_unit_result_t;

typedef es_unit_result_t (*es_unit_func_t)(const char *);

#ifdef ES_OS_LINUX
#define es_unit(N) extern es_unit_result_t es_unit_test_##N(const char *func_name); es_unit_result_t es_unit_test_##N(const char *func_name)
#endif // ES_OS_LINUX
#define es_unit_fail() return (es_unit_result_t) { false, __LINE__, __FILE__, func_name };
#define es_unit_success() return (es_unit_result_t) { true, __LINE__, __FILE__, func_name };

ES_API void es_unit_test(es_da(const char *) source_files, const char *library_file);

/*=========================*/
// Implementation
/*=========================*/

#ifdef ES_IMPL
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

void _es_da_insert_impl(void **arr, const void *data, usize_t index) {
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

    es_memcpy(dest, src, (head->count - index) * head->size);
    es_memcpy(src, data, head->size);
    head->count++;
}

void _es_da_remove_impl(void **arr, usize_t index, void *output) {
    es_assert(arr != NULL, "Can't remove from a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (index + 1) * head->size;
    void *dest = ptr + (index) * head->size;

    if (output) {
        es_memcpy(output, dest, head->size);
    }

    es_memcpy(dest, src, (head->count - index - 1) * head->size);

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

    es_memcpy(dest, src, head->size);
    es_memcpy(src, data, head->size);
    head->count++;
}

void _es_da_remove_fast_impl(void **arr, usize_t index, void *output) {
    es_assert(arr != NULL, "Can't remove from a NULL array.", NULL);

    _es_da_header_t *head = _es_da_head(*arr);

    u8_t *ptr = *arr;
    void *src = ptr + (head->count - 1) * head->size;
    void *dest = ptr + (index) * head->size;

    if (output) {
        es_memcpy(output, dest, head->size);
    }

    es_memcpy(dest, src, head->size);

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

    es_memcpy(dest, src, (head->count - index) * head->size);
    if (data != NULL) {
        es_memcpy(src, data, head->size * count);
    } else {
        es_memset(src, 0, head->size * count);
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
        es_memcpy(output, ptr + index * head->size, head->size * count);
    }

    es_memcpy(dest, src, (head->count - index - count) * head->size);
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
    else if (count < 0 && head->count + count <= (head->cap >> 1)) {
        while (head->count + count > (head->cap >> 1) && head->cap > _ES_DA_INIT_CAP) {
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
// Utils
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
    usize_t i = es_cstr_len(str);
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
    usize_t v0, v1, vec2, vec3, data;

    v0 = ((((usize_t) 0x736f6d65 << 16) << 16) + 0x70736575) ^  seed;
    v1 = ((((usize_t) 0x646f7261 << 16) << 16) + 0x6e646f6d) ^ ~seed;
    vec2 = ((((usize_t) 0x6c796765 << 16) << 16) + 0x6e657261) ^  seed;
    vec3 = ((((usize_t) 0x74656462 << 16) << 16) + 0x79746573) ^ ~seed;

    // Rotate left.
    #define es_siprotl(val, n) (((val) << (n) | ((val) >> (sizeof(usize_t) * 8 - (n)))))
    #define es_sipround() do {\
        v0 += v1; v1 = es_siprotl(v1, 13); v1 ^= v0; v0 = es_siprotl(v0, sizeof(usize_t) * 4); \
        vec2 += vec3; vec3 = es_siprotl(vec3, 16); vec3 ^= vec2; \
        vec2 += v1; v1 = es_siprotl(v1, 17); v1 ^= vec2; vec2 = es_siprotl(vec2, sizeof(usize_t) * 4); \
        v0 += vec3; vec3 = es_siprotl(vec3, 21); vec3 ^= v0; \
    } while (0)

    usize_t i;
    for (i = 0; i+sizeof(usize_t) <= len; i += sizeof(usize_t), d += sizeof(usize_t)) {
        data = d[0] | (d[1] << 8) | (d[2] << 16) | (d[3] << 24);
        data |= (usize_t) (d[4] | (d[5] << 8) | (d[6] << 16) | (d[7] << 24)) << 16 << 16;

        vec3 ^= data;
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
    vec3 ^= data;
    for (i = 0; i < ES_SIPHASH_C_ROUNDS; i++) {
        es_sipround();
    }
    v0 ^= data;
    vec2 ^= 0xff;
    for (i = 0; i < ES_SIPHASH_D_ROUNDS; i++) {
        es_sipround();
    }

    return v0^v1^vec2^vec3;
    #undef es_siprotl
    #undef es_sipround
}

#ifdef ES_OS_LINUX
f64_t es_get_time(void) {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    return (t.tv_sec * 1E9 + t.tv_nsec) * 1E-6;
}

void es_sleep(usize_t ms) {
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, 0);
}
#endif // ES_OS_LINUX
#ifdef ES_OS_WIN32
static f32_t _es_clock_freq = 0.0f;
static LARGE_INTEGER start_time = 0;

f64_t es_get_time(void) {
    if (_es_clock_freq == 0) {
        _win32_clock_setup();
    }

	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (f32_t) now.QuadPart * clock_freq;
}

void es_sleep(usize_t ms) {
	Sleep(ms);
}

void _es_win32_clock_setup(void) {
	u64_t freq;
	QueryPerformanceFrequency(&freq);
	clock_freq = 1.0f / (f32_t) freq;
	QueryPerformanceCounter(&start_time);
}
#endif // ES_OS_WIN32

/*=========================*/
// Hash table
/*=========================*/

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

/*=========================*/
// Threading
/*=========================*/

//
// Linux
//
#ifdef ES_OS_LINUX
es_thread_t es_thread(es_thread_proc_t proc, void *arg) {
    typedef void *(*_es_pthread_proc)(void *);
    es_thread_t thread = 0;
    // I'm sorry for the pointer conversion on the proc. It's necessary.
    pthread_create(&thread, NULL, *(_es_pthread_proc *) &proc, arg);

    return thread;
}

es_thread_t es_thread_get_self(void) {
    return (es_thread_t) { pthread_self() };
}

void es_thread_wait(es_thread_t thread) {
    pthread_join(thread, NULL);
}

es_mutex_t es_mutex_init(void) {
    es_mutex_t mutex = {0};
    mutex.handle = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    return mutex;
}
void es_mutex_free(es_mutex_t *mutex)   { pthread_mutex_destroy(&mutex->handle); }
void es_mutex_lock(es_mutex_t *mutex)   { pthread_mutex_lock(&mutex->handle); }
void es_mutex_unlock(es_mutex_t *mutex) { pthread_mutex_unlock(&mutex->handle); }

#endif // ES_OS_LINUX

//
// Windows
//
#ifdef ES_OS_WIN32
es_thread_t es_thread(es_thread_proc_t proc, void *arg) {
    typedef usize_t (*_es_win32_thread_proc)(void *);
    es_thread_t thread = 0;
    HANDLE handle = CreateThread(NULL, 0, *(_es_win32_thread_proc *) &proc, arg, 0, &thread);
    CloseHandle(handle);
    return thread;
}

es_thread_t es_thread_get_self(void) {
    return GetCurrentThreadId();
}

void es_thread_wait(es_thread_t thread) {
    HANDLE handle = OpenThread(SYNCHRONIZE, false, thread);
    if (handle == NULL) {
        return;
    }
    WaitForSingleObject(handle, INFINITE);
}

es_mutex_t es_mutex_init(void) {
    es_mutex_t mutex = {0};
    mutex.handle = CreateMutex(NULL, false, NULL);
    return mutex;
}

void es_mutex_free(es_mutex_t *mutex) {
    CloseHandle(mutex->handle);
}

void es_mutex_lock(es_mutex_t *mutex) {
    WaitForSingleObject(mutex->handle, INFINITE);
}

void es_mutex_unlock(es_mutex_t *mutex) {
    ReleaseMutex(mutex->handle);
}

#endif // ES_OS_WIN32

/*=========================*/
// Strings
/*=========================*/

es_str_t es_strn(const char *str, usize_t len) {
    _es_str_head_t *head = es_malloc(sizeof(_es_str_head_t) + len + 1);
    head->len = len;
    head->valid = true;

    es_str_t ptr = _es_str_ptr(head);
    es_memcpy(ptr, str, len);
    ptr[len] = '\0';

    return ptr;
}

es_str_t es_str(const char *str) {
    return es_strn(str, es_cstr_len(str));
}

es_str_t es_str_empty(void) {
    return es_strn("", 0);
}

es_str_t es_str_reserve(usize_t len) {
    _es_str_head_t *head = es_malloc(sizeof(_es_str_head_t) + len + 1);
    head->len = len;
    head->valid = true;

    es_str_t ptr = _es_str_ptr(head);
    es_memset(ptr, 0, len + 1);

    return ptr;
}

void es_str_free(es_str_t *str) {
    es_free(_es_str_head(*str));
    *str = NULL;
}

b8_t es_str_valid(const es_str_t str) {
    return _es_str_head(str)->valid;
}

usize_t es_str_len(const es_str_t str) {
    return _es_str_head(str)->len;
}

es_str_t es_str_concat_len(es_str_t str, const char *end, usize_t len) {
    usize_t orig_len = es_str_len(str);
    str = _es_str_resize(str, len);
    if (!es_str_valid(str)) {
        return str;
    }
    es_memcpy(str + orig_len, end, len);
    str[orig_len + len] = '\0';
    return str;
}

es_str_t es_str_concat(es_str_t str, const char *end) {
    return es_str_concat_len(str, end, es_cstr_len(end));
}

es_str_t es_str_sub_start(es_str_t str, usize_t len) {
    return es_str_sub_len(str, 0, len);
}

es_str_t es_str_sub_end(es_str_t str, usize_t len) {
    return es_str_sub_len(str, es_str_len(str) - len, len);
}

es_str_t es_str_sub_index(es_str_t str, usize_t start, usize_t end) {
    es_assert(start <= end, "Starting index can't be greater than end index.", NULL);
    return es_str_sub_len(str, start, end - start);
}

es_str_t es_str_sub_len(es_str_t str, usize_t start, usize_t len) {
    es_str_t result = es_strn(str + start, len);
    return result;
}

es_str_t _es_str_resize(es_str_t str, usize_t len) {
    _es_str_head_t *head = _es_str_head(str);
    _es_str_head_t *new_head = realloc(head, sizeof(_es_str_head_t) + head->len + len);
    if (new_head == NULL) {
        head->valid = false;
    } else {
        head = new_head;
        head->len += len;
    }
    return _es_str_ptr(head);
}

usize_t es_cstr_len(const char *str) {
    usize_t len = 0;
    while (str[len++ + 1] != '\0');
    return len;
}

i32_t es_cstr_cmp_len(const char *a, const char *b, usize_t len) {
    while (len-- - 1) {
        if (*a != *b) {
            break;
        }

        a++;
        b++;
    }

    return *(const u8_t *) a - *(const u8_t *) b;
}

i32_t es_cstr_cmp(const char *a, const char *b) {
    while (*a) {
        if (*a != *b) {
            break;
        }

        a++;
        b++;
    }

    return *(const u8_t *) a - *(const u8_t *) b;
}

b8_t es_is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\f' || c == '\r' || c == '\n');
}

b8_t es_is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

b8_t es_is_digit(char c) {
    return (c >= '0' && c <= '9');
}

/*=========================*/
// Filesystem
/*=========================*/

b8_t es_file_write(const char *filepath, const char *content) {
    FILE *stream = fopen(filepath, "wb");
    if (stream == NULL) {
        return false;
    }

    fwrite(content, es_cstr_len(content), 1, stream);
    fclose(stream);

    return true;
}

b8_t es_file_append(const char *filepath, const char *content) {
    FILE *stream = fopen(filepath, "ab");
    if (stream == NULL) {
        return false;
    }

    fwrite(content, es_cstr_len(content), 1, stream);
    fclose(stream);

    return true;
}

es_str_t es_file_read(const char *filepath) {
    FILE *stream = fopen(filepath, "rb");

    fseek(stream, 0, SEEK_END);
    usize_t len = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    // Allocate an extra byte for null terminator.
    es_str_t buffer = es_str_reserve(len);
    fread(buffer, len, 1, stream);

    fclose(stream);

    return buffer;
}

b8_t es_file_exists(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    return f != NULL;
}

/*=========================*/
// Math
/*=========================*/

mat3_t mat3_inverse(mat3_t mat) {
    // Implementation based on: https://www.mathsisfun.com/algebra/matrix-inverse-minors-cofactors-adjugate.html

    //
    // I have no idea how or why any of this works. I'm just happy it is.
    //

    f32_t f[3][3];
    es_memcpy(f, &mat, sizeof(mat3_t));

    f32_t f00 = f[0][0], f01 = f[0][1], f02 = f[0][2];
    f32_t f10 = f[1][0], f11 = f[1][1], f12 = f[1][2];
    f32_t f20 = f[2][0], f21 = f[2][1], f22 = f[2][2];

    // Step 1: Matrix of minors
    f32_t minor[3][3] = {
        {mat2_determinate(mat2f(f11, f21, f12, f22)), mat2_determinate(mat2f(f10, f20, f12, f22)), mat2_determinate(mat2f(f10, f20, f11, f21))},
        {mat2_determinate(mat2f(f01, f21, f02, f22)), mat2_determinate(mat2f(f00, f20, f02, f22)), mat2_determinate(mat2f(f00, f20, f01, f21))},
        {mat2_determinate(mat2f(f01, f11, f02, f12)), mat2_determinate(mat2f(f00, f10, f02, f12)), mat2_determinate(mat2f(f00, f10, f01, f11))},
    };

    // Step 2: Matrix of cofactors
    f32_t cofactor[3][3];
    for (u8_t x = 0; x < 3; x++) {
        for (u8_t y = 0; y < 3; y++) {
            cofactor[x][y] = (x + y) % 2 == 0 ? minor[x][y] : -minor[x][y];
        }
    }

    // Step 3: Adjugate/adjoint
    mat3_t adj_m;
    for (u8_t x = 0; x < 3; x++) {
        for (u8_t y = 0; y < 3; y++) {
            *((f32_t *) &adj_m + (x + y * 3)) = cofactor[x][y];
        }
    }

    // Step 4: Multiply by 1 / determinant
    f32_t det = f00 * minor[0][0] - f10 * minor[1][0] + f20 * minor[2][0];

    mat3_t finished = mat3_muls(adj_m, 1.0f / det);

    return finished;
}

mat4_t mat4_inverse(mat4_t mat) {
    f32_t f[4][4];
    es_memcpy(f, &mat, sizeof(mat4_t));

    // Step 1: Matrix of minors
    mat4_t minor;
    mat3_t det;
    u8_t det_i = 0;
    for (u8_t x = 0; x < 4; x++) {
        for (u8_t y = 0; y < 4; y++) {
            for (i8_t i = 0; i < 4; i++) {
                for (i8_t j = 0; j < 4; j++) {
                    if (i == x) { continue; }
                    if (j == y) { continue; }
                    ((f32_t *) &det)[det_i++] = f[i][j];
                }
            }
            det_i = 0;
            ((f32_t *) &minor)[x + y * 4] = mat3_determinate(det);
        }
    }

    // Step 2: Matrix of cofactors
    mat4_t cofactor;
    for (u8_t x = 0; x < 4; x++) {
        for (u8_t y = 0; y < 4; y++) {
            ((f32_t *) &cofactor)[x + y * 4] = ((x + y) % 2 == 0) ? ((f32_t *) &minor)[x + y * 4] : -((f32_t *) &minor)[x + y * 4];
        }
    }

    // Step 3: Adjugate/adjoint
    mat4_t adj;
    for (u8_t x = 0; x < 4; x++) {
        for (u8_t y = 0; y < 4; y++) {
            ((f32_t *) &adj)[x + y * 4] = ((f32_t *) &cofactor)[y + x * 4];
        }
    }

    // Step 4: Multiply by 1 / determinant
    f32_t mat_det = mat.i.x * minor.i.x - mat.j.x * minor.j.x + mat.k.x * minor.k.x - mat.l.x * minor.l.x;

    mat4_t finished = mat4_muls(adj, 1.0f / mat_det);

    return finished;
}

/*=========================*/
// Profiler
/*=========================*/

_es_profile_t _es_root_profile = {0};
_es_profile_t *_es_curr_profile = &_es_root_profile;

_es_profile_t _es_profile_new(const char *name) {
    _es_profile_t prof = {
        .parent = _es_curr_profile,
        .name = name,
    };
    return prof;
}

void _es_profile_begin(const char *name) {
    // No binary search because profile order should be preserved.
    // Shouldn't matter since there shouldn't be a large number or profiles.
    b8_t registred = false;
    for (usize_t i = 0; i < es_da_count(_es_curr_profile->children); i++) {
        // Profile already registered.
        if (es_cstr_cmp(_es_curr_profile->children[i].name, name) == 0) {
            _es_curr_profile = &_es_curr_profile->children[i];
            registred = true;
            break;
        }
    }
    // Register a new profile.
    if (!registred) {
        es_da_push(_es_curr_profile->children, _es_profile_new(name));
        _es_curr_profile = &es_da_last(_es_curr_profile->children);
    }

    _es_curr_profile->t0 = es_get_time();
    _es_curr_profile->runs++;
}

void _es_profile_end(void) {
    _es_curr_profile->time += es_get_time() - _es_curr_profile->t0;
    _es_curr_profile = _es_curr_profile->parent;
}

void _es_profile_print(const _es_profile_t *prof, usize_t gen) {
    for (usize_t i = 0; i < gen; i++) {
        printf("    ");
    }
    printf("%s: %f %f %d\n", prof->name, prof->time, prof->time / prof->runs, prof->runs);
    for (usize_t i = 0; i < es_da_count(prof->children); i++) {
        _es_profile_print(&prof->children[i], gen + 1);
    }
}

void es_profile_print(void) {
    printf("========== Profile ==========\n");
    printf("Name: total_time avarage_time run_count\n");
    for (usize_t i = 0; i < es_da_count(_es_root_profile.children); i++) {
        _es_profile_print(&_es_root_profile.children[i], 0);
    }
    printf("========== End ==========\n");
}

/*=========================*/
// Windowing
/*=========================*/

//
// Linux
//
#ifdef ES_OS_LINUX
es_window_t *es_window_init(i32_t width, i32_t height, const char *title, b8_t resizable) {
    _es_window_t *window = es_malloc(sizeof(_es_window_t));

    window->is_open = true;
    window->size = vec2(width, height);

    window->display = XOpenDisplay(NULL);
    if (window->display == NULL) {
        es_free(window);
        return NULL;
    }

    i32_t root = DefaultRootWindow(window->display);
    i32_t screen = DefaultScreen(window->display);

    // Check if display is compatible.
    i32_t screen_bit_depth = 24;
    XVisualInfo visinfo = {0};
    if (!XMatchVisualInfo(window->display, screen, screen_bit_depth, TrueColor, &visinfo)) {
        XCloseDisplay(window->display);
        es_free(window);
        return NULL;
    }

    // Configure window attributes.
    XSetWindowAttributes window_attrs = {0};
    window_attrs.background_pixel = 0;
    window_attrs.colormap = XCreateColormap(window->display, root, visinfo.visual, AllocNone);
    // Events the widnow accepts.
    window_attrs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | FocusChangeMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    usize_t attribute_mask = CWBackPixel | CWColormap | CWEventMask;

    // Create window.
    window->window = XCreateWindow(window->display, root, 0, 0, width, height, 0, visinfo.depth, InputOutput, visinfo.visual, attribute_mask, &window_attrs);
    if (!window->window) {
        XDestroyWindow(window->display, window->window);
        XCloseDisplay(window->display);
        es_free(window);
        return NULL;
    }

    // Set title.
    XStoreName(window->display, window->window, title);

    // Window resizability.
    XSizeHints hints = {0};
    if (!resizable) {
        hints.flags      = PMinSize | PMaxSize;
        hints.min_width  = window->size.x;
        hints.min_height = window->size.y;
        hints.max_width  = window->size.x;
        hints.max_height = window->size.y;
        XSetWMNormalHints(window->display, window->window, &hints);
    }

    // Show window.
    XMapWindow(window->display, window->window);
    // Send all X commands to X-server.
    XFlush(window->display);

    // Get close command.
    window->wm_delete_window = XInternAtom(window->display, "WM_DELETE_WINDOW", false);
    if (!XSetWMProtocols(window->display, window->window, &window->wm_delete_window, true)) {
        XDestroyWindow(window->display, window->window);
        XCloseDisplay(window->display);
        es_free(window);
        return NULL;
    }

    // Input.
    XIM xinput_method = XOpenIM(window->display, NULL, NULL, NULL);
    if (!xinput_method) {
        printf("Input method could not be opened\n");
    }

    XIMStyles *styles = NULL;
    if (XGetIMValues(xinput_method, XNQueryInputStyle, &styles, NULL) || !styles) {
        printf("Input styles could not be retrieved\n");
    }

    XIMStyle best_match_style = 0;
    for (i32_t i = 0; i < styles->count_styles; i++) {
        XIMStyle style = styles->supported_styles[i];
        if (style == (XIMPreeditNothing | XIMStatusNothing)) {
            best_match_style = style;
            break;
        }
    }
    XFree(styles);

    if (!best_match_style) {
        printf("No matching input style could be determined\n");
    }

    window->input_context = XCreateIC(
        xinput_method,
        XNInputStyle,   best_match_style,
        XNClientWindow, window->window,
        XNFocusWindow,  window->window,
        NULL
    );

    return window;
}

void es_window_free(es_window_t *window) {
    _es_window_t *_window = window;

    XCloseDisplay(_window->display);
    es_free(_window);
}

void es_window_poll_events(es_window_t *window) {
    _es_window_t *_window = window;
    XEvent ev = {0};
    while (XPending(_window->display)) {
        XNextEvent(_window->display, &ev);
        switch (ev.type) {
            case DestroyNotify: {
                XDestroyWindowEvent *e = (XDestroyWindowEvent *) &ev;
                if (e->window == _window->window) {
                    _window->is_open = false;
                }
            } break;
            case ClientMessage: {
                XClientMessageEvent *e = (XClientMessageEvent *) &ev;
                if ((Atom) e->data.l[0] == _window->wm_delete_window) {
                    XDestroyWindow(_window->display, _window->window);
                    _window->is_open = false;
                }
            } break;
            case ConfigureNotify: {
                XConfigureEvent *e = (XConfigureEvent *) &ev;
                if (_window->size.x != e->width || _window->size.y != e->height) {
                    if (_window->resize_callback) {
                        _window->resize_callback(window, e->width, e->height);
                    }
                }
                _window->size.x = e->width;
                _window->size.y = e->height;
            } break;

            case KeyPress:
            case KeyRelease: {
                XKeyEvent *e = (XKeyEvent *) &ev;
                KeySym sym;
                // Pass symbol because it crashed when pressing åäö without passing it into the function.
                // i32_t symbol = 0;
                // Xutf8LookupString(_window->input_context, e, (char *) &symbol, sizeof(KeySym), &sym, NULL);
                sym = XkbKeycodeToKeysym(_window->display, e->keycode, 0, 0);
                es_key_t key = _es_window_translate_keysym(sym);

                // Check what event key action was performed.
                // NOTE: This is useless since key repeats are off because this doesn't work. A better solution is needed.
                b8_t is_repeat = (_window->prev_key_event.time == e->time && _window->prev_key_event.keycode == e->keycode);
                _window->prev_key_event = *e;
                es_key_action_t action = -1;
                if (is_repeat) {
                    action = ES_KEY_ACTION_REPEAT;
                } else {
                    action = e->type == KeyPress;
                }

                if (_window->key_callback) {
                    // Don't call the callback if the key isn't supported.
                    if (key != -1) {
                        _window->key_callback(window, key, e->state, action);
                    }
                }
            } break;

            case ButtonPress:
            case ButtonRelease: {
                XButtonEvent *e = (XButtonEvent *) &ev;
                es_key_action_t action = e->type == ButtonPress ? ES_KEY_ACTION_PRESS : ES_KEY_ACTION_RELEASE;
                es_button_t button = ES_BUTTON_COUNT;
                if (e->button < 4) {
                    button = e->button - 1;
                }
                if (_window->mouse_button_callback && button != ES_BUTTON_COUNT) {
                    _window->mouse_button_callback(window, e->button - 1, action);
                }
                i32_t scroll_offset = e->button == Button5 ? -1 : e->button == Button4 ? 1 : 0;
                if (_window->scroll_callback && scroll_offset != 0 && action == ES_KEY_ACTION_PRESS) {
                    _window->scroll_callback(window, scroll_offset);
                }
            } break;

            case MotionNotify: {
                XMotionEvent *e = (XMotionEvent *) &ev;
                if (_window->cursor_position_callback) {
                    _window->cursor_position_callback(window, e->x, e->y);
                }
            } break;

            case FocusIn:
            case FocusOut: {
                // Only disable key repeates when window is in focuse because X disables it system wid for some reason.
                XFocusChangeEvent *e = (XFocusChangeEvent *) &ev;
                if (e->type == FocusIn) {
                    XAutoRepeatOff(_window->display);
                } else {
                    XAutoRepeatOn(_window->display);
                }
            }
        }
    }
}

#ifdef ES_VULKAN
VkSurfaceKHR es_window_vulkan_surface(const es_window_t *window, VkInstance instance) {
    VkSurfaceKHR surface;

    const _es_window_t *_window = window;

    VkXlibSurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = NULL;
    surface_create_info.flags = 0;
    surface_create_info.dpy = _window->display;
    surface_create_info.window = _window->window;

    VkResult result = vkCreateXlibSurfaceKHR(instance, &surface_create_info, NULL, &surface);
    if (result != VK_SUCCESS) {
        return NULL;
    }

    return surface;
}
#endif // ES_VULKAN

es_key_t _es_window_translate_keysym(KeySym keysym) {
    switch (keysym) {
        // Letters
        // Generation code:
        //     for (u32_t i = 0; i < 26; i++) {
        //         printf("case XK_%c:\ncase XK_%c:\n    return ES_KEY_%c;\n", 'a' + i, 'A' + i, 'A' + i);
        //     }
        case XK_a:
        case XK_A:
            return ES_KEY_A;
        case XK_b:
        case XK_B:
            return ES_KEY_B;
        case XK_c:
        case XK_C:
            return ES_KEY_C;
        case XK_d:
        case XK_D:
            return ES_KEY_D;
        case XK_e:
        case XK_E:
            return ES_KEY_E;
        case XK_f:
        case XK_F:
            return ES_KEY_F;
        case XK_g:
        case XK_G:
            return ES_KEY_G;
        case XK_h:
        case XK_H:
            return ES_KEY_H;
        case XK_i:
        case XK_I:
            return ES_KEY_I;
        case XK_j:
        case XK_J:
            return ES_KEY_J;
        case XK_k:
        case XK_K:
            return ES_KEY_K;
        case XK_l:
        case XK_L:
            return ES_KEY_L;
        case XK_m:
        case XK_M:
            return ES_KEY_M;
        case XK_n:
        case XK_N:
            return ES_KEY_N;
        case XK_o:
        case XK_O:
            return ES_KEY_O;
        case XK_p:
        case XK_P:
            return ES_KEY_P;
        case XK_q:
        case XK_Q:
            return ES_KEY_Q;
        case XK_r:
        case XK_R:
            return ES_KEY_R;
        case XK_s:
        case XK_S:
            return ES_KEY_S;
        case XK_t:
        case XK_T:
            return ES_KEY_T;
        case XK_u:
        case XK_U:
            return ES_KEY_U;
        case XK_v:
        case XK_V:
            return ES_KEY_V;
        case XK_w:
        case XK_W:
            return ES_KEY_W;
        case XK_x:
        case XK_X:
            return ES_KEY_X;
        case XK_y:
        case XK_Y:
            return ES_KEY_Y;
        case XK_z:
        case XK_Z:
            return ES_KEY_Z;

        // Numbers
        // Generation code:
        //     for (u32_t i = 0; i < 10; i++) {
        //         printf("case XK_%c:\n    return ES_KEY_%c;\n", '0' + i, '0' + i);
        //     }
        case XK_0: return ES_KEY_0;
        case XK_1: return ES_KEY_1;
        case XK_2: return ES_KEY_2;
        case XK_3: return ES_KEY_3;
        case XK_4: return ES_KEY_4;
        case XK_5: return ES_KEY_5;
        case XK_6: return ES_KEY_6;
        case XK_7: return ES_KEY_7;
        case XK_8: return ES_KEY_8;
        case XK_9: return ES_KEY_9;

        // Function keys
        // Generation code:
        //     for (u32_t i = 0; i < 24; i++) {
        //         printf("case XK_F%d: return ES_KEY_F%d;\n", i + 1, i + 1);
        //     }
        case XK_F1:  return ES_KEY_F1;
        case XK_F2:  return ES_KEY_F2;
        case XK_F3:  return ES_KEY_F3;
        case XK_F4:  return ES_KEY_F4;
        case XK_F5:  return ES_KEY_F5;
        case XK_F6:  return ES_KEY_F6;
        case XK_F7:  return ES_KEY_F7;
        case XK_F8:  return ES_KEY_F8;
        case XK_F9:  return ES_KEY_F9;
        case XK_F10: return ES_KEY_F10;
        case XK_F11: return ES_KEY_F11;
        case XK_F12: return ES_KEY_F12;
        case XK_F13: return ES_KEY_F13;
        case XK_F14: return ES_KEY_F14;
        case XK_F15: return ES_KEY_F15;
        case XK_F16: return ES_KEY_F16;
        case XK_F17: return ES_KEY_F17;
        case XK_F18: return ES_KEY_F18;
        case XK_F19: return ES_KEY_F19;
        case XK_F20: return ES_KEY_F20;
        case XK_F21: return ES_KEY_F21;
        case XK_F22: return ES_KEY_F22;
        case XK_F23: return ES_KEY_F23;
        case XK_F24: return ES_KEY_F24;

        case XK_Escape:    return ES_KEY_ESC;
        case XK_Tab:       return ES_KEY_TAB;
        case XK_Return:    return ES_KEY_ENTER;
        case XK_space:     return ES_KEY_SPACE;
        case XK_BackSpace: return ES_KEY_BACKSPACE;
        case XK_Caps_Lock: return ES_KEY_CAPS_LOCK;

        case XK_Left:  return ES_KEY_LEFT;
        case XK_Down:  return ES_KEY_DOWN;
        case XK_Up:    return ES_KEY_UP;
        case XK_Right: return ES_KEY_RIGHT;

        // Mod keys.
        case XK_Shift_L:   return ES_KEY_SHIFT_L;
        case XK_Shift_R:   return ES_KEY_SHIFT_R;
        case XK_Control_L: return ES_KEY_CTRL_L;
        case XK_Control_R: return ES_KEY_CTRL_R;
        case XK_Alt_L:     return ES_KEY_ALT_L;
        case XK_Alt_R:     return ES_KEY_ALT_R;
        case XK_Super_L:   return ES_KEY_SUPER_L;
        case XK_Super_R:   return ES_KEY_SUPER_R;

        // Symbols.
        case XK_period:       return ES_KEY_PERIOD;
        case XK_comma:        return ES_KEY_COMMA;
        case XK_semicolon:    return ES_KEY_SEMICOLON;
        case XK_plus:         return ES_KEY_PLUS;
        case XK_minus:        return ES_KEY_MINUS;
        case XK_equal:        return ES_KEY_EQUAL;
        case XK_bracketleft:  return ES_KEY_BRACKET_L;
        case XK_bracketright: return ES_KEY_BRACKET_R;
        case XK_apostrophe:   return ES_KEY_APOSTROPHE;
        case XK_quotedbl:     return ES_KEY_QUOTE;
        case XK_slash:        return ES_KEY_SLASH;
        case XK_bar:          return ES_KEY_PIPE;
        case XK_backslash:    return ES_KEY_BACKSLASH;
        case XK_dead_grave:
        case XK_grave: return ES_KEY_GRAVE;

        default: return ES_KEY_NULL;
    }
}
#endif // ES_OS_LINUX

//
// Windows
//
#ifdef ES_OS_WIN32
es_window_t *es_window_init(i32_t width, i32_t height, const char *title, b8_t resizable) {
    _es_window_t *window = es_malloc(sizeof(_es_window_t));
    window->is_open = true;
    window->size = vec2(width, height);

    window->resize_callback = NULL;
    window->key_callback = NULL;
    window->mouse_button_callback = NULL;
    window->cursor_position_callback = NULL;
    window->scroll_callback = NULL;

    window->instance = GetModuleHandleA(0);

    const char *class_name = "earthshine_window_class";
    WNDCLASSA wc = {0};
    wc.lpfnWndProc = _es_window_process_message;
    wc.hInstance = window->instance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // Allocate extra bytes for window pointer.
    wc.cbWndExtra = sizeof(window);

    if (!RegisterClassA(&wc)) {
        es_free(window);
        return NULL;
    }

    DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX;
    if (resizable) {
        style |= WS_MAXIMIZEBOX;
        style |= WS_THICKFRAME;
    }

    // Correct window sizing.
    RECT border_rect = {0};
    AdjustWindowRectEx(&border_rect, style, 0, 0);
    u32_t border_margin_x = border_rect.right - border_rect.left;
    u32_t border_margin_y = border_rect.bottom - border_rect.top;

    // Create window.
    window->window = CreateWindowExA(
        0,
        class_name,
        title,
        style,
        CW_USEDEFAULT, CW_USEDEFAULT, // X, Y
        width + border_margin_x, height + border_margin_y,
        0,
        0,
        window->instance,
        0
    );
    if (window->window == NULL) {
        es_free(window);
        return NULL;
    }

    SetWindowLongPtrA(window->window, 0, (LONG_PTR) window);

    // Present window.
    ShowWindow(window->window, SW_SHOW);

    return window;
}

void es_window_free(es_window_t *window) {
    _es_window_t *_window = window;
    DestroyWindow(_window->window);
}

void es_window_poll_events(es_window_t *window) {
    _es_window_t *_window = window;

    MSG msg = {0};
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE) > 0) {
        switch (msg.message) {
            case WM_QUIT:
                _window->is_open = false;
                break;
            default:
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
                break;
        }
    }
}

LRESULT CALLBACK _es_window_process_message(HWND hwnd, u32_t msg, WPARAM w_param, LPARAM l_param) {
    _es_window_t *window = (_es_window_t *) GetWindowLongPtrA(hwnd, 0);
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_DESTROY: {
            PostQuitMessage(0);
            return 0;
        }
        case WM_SIZE: {
            if (window->resize_callback != NULL) {
                RECT r;
                GetClientRect(hwnd, &r);
                window->size.x = r.right - r.left;
                window->size.y = r.bottom - r.top;
                window->resize_callback(window, window->size.x, window->size.y);
            }
        } break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            es_key_action_t action = (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN);
            es_button_t button = ES_BUTTON_COUNT;
            switch (msg)
            {
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
                button = ES_BUTTON_LEFT;
                break;
            case WM_MBUTTONDOWN:
            case WM_MBUTTONUP:
                button = ES_BUTTON_MIDDLE;
                break;
            case WM_RBUTTONDOWN:
            case WM_RBUTTONUP:
                button = ES_BUTTON_RIGHT;
                break;
            }
            if (button != ES_BUTTON_COUNT && window->mouse_button_callback != NULL) {
                window->mouse_button_callback(window, button, action);
            }
        } break;

        case WM_MOUSEWHEEL: {
            if (window->scroll_callback != NULL) {
                i32_t offset = GET_WHEEL_DELTA_WPARAM(w_param);
                if (offset == 0) {
                    break;
                }
                offset = (offset < 0) ? -1 : 1;
                window->scroll_callback(window, offset);
            }
        } break;

        case WM_MOUSEMOVE: {
            if (window->cursor_position_callback != NULL) {
                i32_t x = GET_X_LPARAM(l_param);
                i32_t y = GET_Y_LPARAM(l_param);
                window->cursor_position_callback(window, x, y);
            }
        } break;

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP: {
            es_key_action_t action = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            es_key_t key = (u16_t) w_param;
            key = _es_window_translate_scancode(key);

            // Check for extended scan code.
            b8_t is_extended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;

            if (w_param == VK_MENU) {
                key = is_extended ? ES_KEY_ALT_R : ES_KEY_ALT_L;
            } else if (w_param == VK_SHIFT) {
                u32_t left_shift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
                u32_t scancode = ((l_param & (0xFF << 16)) >> 16);
                key = scancode == left_shift ? ES_KEY_SHIFT_L : ES_KEY_SHIFT_R;
            } else if (w_param == VK_CONTROL) {
                key = is_extended ? ES_KEY_CTRL_R : ES_KEY_CTRL_L;
            }

            if (window->key_callback != NULL && key != ES_KEY_NULL) {
                window->key_callback(window, key, 0, action);
            }

            // Prevent default windows behavior.
            return 1;
        }
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#ifdef ES_VULKAN
VkSurfaceKHR es_window_vulkan_surface(const es_window_t *window, VkInstance instance) {
    VkSurfaceKHR surface;

    const _es_window_t *_window = window;

    VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surface_create_info.pNext = NULL;
    surface_create_info.flags = 0;
    surface_create_info.hinstance = _window->instance;
    surface_create_info.hwnd = _window->window;

    VkResult result = vkCreateXlibSurfaceKHR(instance, &surface_create_info, NULL, &surface);
    if (result != VK_SUCCESS) {
        return NULL;
    }

    return surface;
}
#endif // ES_VULKAN

es_key_t _es_window_translate_scancode(u16_t scancode) {
    switch (scancode)
    {
        case 0x41: return ES_KEY_A;
        case 0x42: return ES_KEY_B;
        case 0x43: return ES_KEY_C;
        case 0x44: return ES_KEY_D;
        case 0x45: return ES_KEY_E;
        case 0x46: return ES_KEY_F;
        case 0x47: return ES_KEY_G;
        case 0x48: return ES_KEY_H;
        case 0x49: return ES_KEY_I;
        case 0x4a: return ES_KEY_J;
        case 0x4b: return ES_KEY_K;
        case 0x4c: return ES_KEY_L;
        case 0x4d: return ES_KEY_M;
        case 0x4e: return ES_KEY_N;
        case 0x4f: return ES_KEY_O;
        case 0x50: return ES_KEY_P;
        case 0x51: return ES_KEY_Q;
        case 0x52: return ES_KEY_R;
        case 0x53: return ES_KEY_S;
        case 0x54: return ES_KEY_T;
        case 0x55: return ES_KEY_U;
        case 0x56: return ES_KEY_V;
        case 0x57: return ES_KEY_W;
        case 0x58: return ES_KEY_X;
        case 0x59: return ES_KEY_Y;
        case 0x5a: return ES_KEY_Z;

        case 0x30: return ES_KEY_0;
        case 0x31: return ES_KEY_1;
        case 0x32: return ES_KEY_2;
        case 0x33: return ES_KEY_3;
        case 0x34: return ES_KEY_4;
        case 0x35: return ES_KEY_5;
        case 0x36: return ES_KEY_6;
        case 0x37: return ES_KEY_7;
        case 0x38: return ES_KEY_8;
        case 0x39: return ES_KEY_9;

        case 0x70: return ES_KEY_F1;
        case 0x71: return ES_KEY_F2;
        case 0x72: return ES_KEY_F3;
        case 0x73: return ES_KEY_F4;
        case 0x74: return ES_KEY_F5;
        case 0x75: return ES_KEY_F6;
        case 0x76: return ES_KEY_F7;
        case 0x77: return ES_KEY_F8;
        case 0x78: return ES_KEY_F9;
        case 0x79: return ES_KEY_F10;
        case 0x7a: return ES_KEY_F11;
        case 0x7b: return ES_KEY_F12;
        case 0x7c: return ES_KEY_F13;
        case 0x7d: return ES_KEY_F14;
        case 0x7e: return ES_KEY_F15;
        case 0x7f: return ES_KEY_F16;
        case 0x80: return ES_KEY_F17;
        case 0x81: return ES_KEY_F18;
        case 0x82: return ES_KEY_F19;
        case 0x83: return ES_KEY_F20;
        case 0x84: return ES_KEY_F21;
        case 0x85: return ES_KEY_F22;
        case 0x86: return ES_KEY_F23;
        case 0x87: return ES_KEY_F24;

        case 0x1b: return ES_KEY_ESC;
        case 0x9:  return ES_KEY_TAB;
        case 0x0d: return ES_KEY_ENTER;
        case 0x20: return ES_KEY_SPACE;
        case 0x8:  return ES_KEY_BACKSPACE;
        case 0x14: return ES_KEY_CAPS_LOCK;

        case 0x25: return ES_KEY_LEFT;
        case 0x28: return ES_KEY_DOWN;
        case 0x26: return ES_KEY_UP;
        case 0x27: return ES_KEY_RIGHT;

        case 0xa0: return ES_KEY_SHIFT_L;
        case 0xa1: return ES_KEY_SHIFT_R;
        case 0xa2: return ES_KEY_CTRL_L;
        case 0xa3: return ES_KEY_CTRL_R;
        case 0xa4: return ES_KEY_ALT_L;
        case 0xa5: return ES_KEY_ALT_R;
        case 0x5b: return ES_KEY_SUPER_L;
        case 0x5c: return ES_KEY_SUPER_R;

        case 0x3b: return ES_KEY_SEMICOLON;
        case 0xde: return ES_KEY_APOSTROPHE;
        case 0xbb: return ES_KEY_EQUAL;
        case 0xbc: return ES_KEY_COMMA;
        case 0xbd: return ES_KEY_MINUS;
        case 0xbe: return ES_KEY_PERIOD;
        case 0xbf: return ES_KEY_SLASH;
        case 0xc0: return ES_KEY_GRAVE;
        case 0xdb: return ES_KEY_BRACKET_L;
        case 0xdc: return ES_KEY_PIPE;
        case 0xdd: return ES_KEY_BRACKET_R;

        default: return ES_KEY_NULL;
    }
}
#endif // ES_OS_WIN32

//
// Getters
//

b8_t es_window_is_open(es_window_t *window) {
    _es_window_t *_window = window;
    return _window->is_open;
}

vec2_t es_window_get_size(es_window_t *window) {
    _es_window_t *_window = window;
    return _window->size;
}

//
// Callbacks (not OS specific)
//

void es_window_set_resize_callback(es_window_t *window, es_window_resize_callback_t callback) {
    _es_window_t *_window = window;
    _window->resize_callback = callback;
}

void es_window_set_key_callback(es_window_t *window, es_window_key_callback_t callback) {
    _es_window_t *_window = window;
    _window->key_callback = callback;
}

void es_window_set_mouse_button_callback(es_window_t *window, es_window_mouse_button_callback_t callback) { 
    _es_window_t *_window = window;
    _window->mouse_button_callback = callback;
}

void es_window_set_cursor_position_callback(es_window_t *window, es_window_cursor_position_callback callback) {
    _es_window_t *_window = window;
    _window->cursor_position_callback = callback;
}

void es_window_set_scroll_callback(es_window_t *window, es_window_scroll_callback callback) {
    _es_window_t *_window = window;
    _window->scroll_callback = callback;
}

/*=========================*/
// Library loading
/*=========================*/

//
// Linux
//
#ifdef ES_OS_LINUX
es_lib_t es_library_init(const char *filepath) {
    es_lib_t lib = {
        .handle = dlopen(filepath, RTLD_LAZY | RTLD_LOCAL),
        .valid = true
    };

    if (lib.handle == NULL) {
        lib.valid = false;
    }

    return lib;
}

es_lib_func_t es_library_function(es_lib_t lib, const char *name) {
    if (!lib.valid) {
        return (es_lib_func_t) {0};
    }

    es_lib_func_t func = { .valid = true };
    // I'm sorry for this conversion.
    *(void **) &func.func = dlsym(lib.handle, name);

    if (func.func == NULL) {
        func.valid = false;
    }

    return func;
}

void es_library_free(es_lib_t *lib) {
    if (!lib->valid) {
        return;
    }

    dlclose(lib->handle);
    lib->handle = NULL;
    lib->valid = false;
}
#endif // ES_OS_LINUX

//
// Windows
//
#ifdef ES_OS_WIN32
es_lib_t es_library_init(const char *filepath) {
    es_lib_t lib = {
        .handle = LoadLibraryA(filepath),
        .valid = true
    };

    if (lib.handle == NULL) {
        lib.valid = false;
    }

    return lib;
}

es_lib_func_t es_library_function(const es_lib_t *lib, const char *name) {
    if (!lib->valid) {
        return (es_lib_func_t) {0};
    }

    es_lib_func_t func = { .valid = true };
    // I'm sorry for this conversion.
    *(void **) &func.func = GetProcAddress(lib->handle, name);

    if (func.func == NULL) {
        func.valid = false;
    }

    return func;
}

void es_library_free(es_lib_t *lib) {
    if (!lib->valid) {
        return;
    }

    FreeLibrary(lib->handle);
    lib->handle = NULL;
    lib->valid = false;
}
#endif // ES_OS_WIN32

/*=========================*/
// Memory
/*=========================*/

void es_memcpy(void *dest, const void *src, usize_t len) {
    u8_t *dest_ptr = dest;
    const u8_t *src_ptr  = src;
    for (usize_t i = 0; i < len; i++) {
        dest_ptr[i] = src_ptr[i];
    }
}

void es_memset(void *dest, i32_t c, usize_t len) {
    u8_t *ptr = dest;
    while (len--) {
        *ptr++ = (u8_t) c;
    }
}

i32_t es_memcmp(const void *a, const void *b, usize_t len) {
    const u8_t *_a = a;
    const u8_t *_b = b;
    for (usize_t i = 0; i < len; i++) {
        if (_a[i] != _b[i]) {
            return (_a[i] <= _b[i] ? -1 : 1);
        }
    }

    return 0;
}

/*=========================*/
// Unit testing
/*=========================*/

void es_unit_test(es_da(const char *) source_files, const char *library_file) {
    es_da(es_str_t) func_names = NULL;

    // Find all function names.
    for (usize_t i = 0; i < es_da_count(source_files); i++) {
        es_str_t content = es_file_read(source_files[i]);
        if (!es_str_valid(content)) {
            printf("Couldn't read source file '%s'.\n", source_files[i]);
            return;
        }

        char *c = content;
        while (*c) {
            if (*c == 'e' && es_cstr_cmp_len(c, "es_unit", 7) == 0) {
                c += 7;
                // Check if it's a declaration or a return statement.
                while (es_is_whitespace(*c)) { c++; }
                if (*c != '(') {
                    continue;
                }
                c++;
                while (es_is_whitespace(*c)) { c++; }
                // Parse function name.
                char *start = c;
                while (*c != ')' || es_is_whitespace(*c)) { c++; }
                char *end = c;
                es_da_push(func_names, es_str_sub_index(content, start - content, end - content));
            }

            c++;
        }

        es_str_free(&content);
    }

    // Call all functions.
    es_lib_t lib = es_library_init(library_file);
    if (!lib.valid) {
        printf("Couldn't open library '%s'\n", library_file);
        return;
    }

    for (usize_t i = 0; i < es_da_count(func_names); i++) {
        es_str_t actual_name = es_str("es_unit_test_");
        actual_name = es_str_concat(actual_name, func_names[i]);
        es_lib_func_t func = es_library_function(lib, actual_name);
        es_str_free(&actual_name);
        if (!func.valid) {
            printf("Couldn't load unit '%s' in library '%s'.\n", func_names[i], library_file);
            continue;
        }
        es_unit_result_t result = ((es_unit_func_t) func.func)(func_names[i]);
        printf("%s:%d:%s: %s\n", result.file, result.line, result.func, result.success ? "success" : "fail");
    }

    es_library_free(&lib);

    es_da_free(func_names);
}
#endif /*ES_IMPL*/
#endif // ES_H
