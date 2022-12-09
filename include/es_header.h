/*====================================================================================================
    * Copyright: Linus Erik Pontus Kåreblom
    * Earthshine: A general purpose single header library
    * File: es.h
    * Version: 1.1
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
#endif // es_malloc
#ifndef es_calloc
#define es_calloc calloc
#endif // es_calloc
#ifndef es_realloc
#define es_realloc realloc
#endif // es_realloc
#ifndef es_free
#define es_free free
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

ES_API void _es_da_init_impl(void **arr, usize_t size);
ES_API void _es_da_insert_impl(void **arr, const void *data, usize_t index);
ES_API void _es_da_remove_impl(void **arr, usize_t index, void *output);
ES_API void _es_da_insert_fast_impl(void **arr, const void *data, usize_t index);
ES_API void _es_da_remove_fast_impl(void **arr, usize_t index, void *output);
ES_API void _es_da_insert_arr_impl(void **arr, const void *data, usize_t count, usize_t index);
ES_API void _es_da_remove_arr_impl(void **arr, usize_t count, usize_t index, void *output);
ES_API void _es_da_free_impl(void **arr);
ES_API void _es_da_resize(void **arr, isize_t count);
ES_API usize_t es_da_count(void *arr);

#define es_da(T) T *

#define es_da_insert(ARR, D, I) do { \
    _es_da_init_impl((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
#define es_da_remove(ARR, I, OUT) _es_da_remove_impl((void **) &(ARR), I, (OUT))
#define es_da_insert_fast(ARR, D, I) do { \
    _es_da_init_impl((void **) &(ARR), sizeof(*(ARR))); \
    __typeof__(D) _es_da_temp_data = D; \
    _es_da_insert_fast_impl((void **) &(ARR), &_es_da_temp_data, (I)); \
} while(0)
#define es_da_remove_fast(ARR, I, OUT) _es_da_remove_fast_impl((void **) &(ARR), (I), (OUT))
#define es_da_push(ARR, D) es_da_insert_fast(ARR, D, es_da_count((ARR)));
#define es_da_pop(ARR, OUT) es_da_remove_fast(ARR, es_da_count((ARR)) - 1, OUT);
#define es_da_insert_arr(ARR, D, C, I) do { \
    _es_da_init_impl((void **) &(ARR), sizeof(*(ARR))); \
    _es_da_insert_arr_impl((void **) &(ARR), (D), (C), (I)); \
} while (0)
#define es_da_remove_arr(ARR, C, I, OUT) _es_da_remove_arr_impl((void **) &(ARR), (C), (I), (OUT))
#define es_da_push_arr(ARR, D, C) es_da_insert_arr(ARR, D, C, es_da_count((ARR)))
#define es_da_pop_arr(ARR, C, OUT) es_da_remove_arr(ARR, C, es_da_count((ARR)) - (C), OUT)
#define es_da_free(ARR) _es_da_free_impl((void **) &(ARR))

/*=========================*/
// Assert
/*=========================*/

ES_API void _es_assert_impl(const char *file, u32_t line, const char *expr_str, b8_t expr, const char *fmt, ...);

#define es_assert(EXPR, FMT, ...) _es_assert_impl(__FILE__, __LINE__, #EXPR, (EXPR), FMT, ##__VA_ARGS__)

/*=========================*/
// Utils
/*=========================*/

#define es_arr_len(ARR) ((sizeof(ARR) / sizeof((ARR)[0])))

/*=========================*/
// Implementation
/*=========================*/

#endif // ES_H
