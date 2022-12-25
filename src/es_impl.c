#include "es_header.h"

#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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
        while (head->count + count >= head->cap >> 1) {
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
    pthread_mutex_init(&mutex.handle, NULL);
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
// Filesystem
/*=========================*/

b8_t es_file_write(const char *filepath, const char *content) {
    FILE *stream = fopen(filepath, "wb");
    if (stream == NULL) {
        return false;
    }

    fwrite(content, strlen(content), 1, stream);
    fclose(stream);

    return true;
}

b8_t es_file_append(const char *filepath, const char *content) {
    FILE *stream = fopen(filepath, "ab");
    if (stream == NULL) {
        return false;
    }

    fwrite(content, strlen(content), 1, stream);
    fclose(stream);

    return true;
}

char *es_file_read(const char *filepath) {
    FILE *stream = fopen(filepath, "rb");

    fseek(stream, 0, SEEK_END);
    usize_t len = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    // Allocate an extra byte for null terminator.
    char *buffer = es_malloc(len + 1);
    fread(buffer, len, 1, stream);
    // Append null terminator.
    buffer[len] = '\0';

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
    memcpy(f, &mat, sizeof(mat3_t));

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
    memcpy(f, &mat, sizeof(mat4_t));

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
        if (strcmp(_es_curr_profile->children[i].name, name) == 0) {
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
