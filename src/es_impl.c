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
// Windowing
/*=========================*/

//
// Linux
//
#ifdef ES_OS_LINUX
es_window_t *es_window_init(i32_t width, i32_t height, const char *title, b8_t resizable) {
    _es_window_t *window = es_malloc(sizeof(_es_window_t));

    window->is_open = true;
    window->width   = width;
    window->height  = height;

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
    window_attrs.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask | FocusChangeMask;
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
        hints.min_width  = window->width;
        hints.min_height = window->height;
        hints.max_width  = window->width;
        hints.max_height = window->height;
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

    return window;
}

void es_window_free(es_window_t *window) {
    _es_window_t *_window = window;

    XCloseDisplay(_window->display);
    es_free(_window);
}

b8_t es_window_is_open(es_window_t *window) {
    return ((_es_window_t *) window)->is_open;
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
                if (_window->width != e->width || _window->height != e->height) {
                    if (_window->resize_callback) {
                        _window->resize_callback(window, e->width, e->height);
                    }
                }
                _window->width = e->width;
                _window->height = e->height;
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

void es_window_resizable(es_window_t *window, b8_t resizable) {
    _es_window_t *_window = window;

    // Window resizability.
    XSizeHints hints = {0};
    if (!resizable) {
        hints.flags      = PMinSize | PMaxSize;
    }
    hints.min_width  = _window->width;
    hints.min_height = _window->height;
    hints.max_width  = _window->width;
    hints.max_height = _window->height;
    XSetWMNormalHints(_window->display, _window->window, &hints);
}

void es_window_callback_resize(es_window_t *window, es_window_resize_callback_t callback) {
    _es_window_t *_window = window;
    _window->resize_callback = callback;
}
#endif // ES_OS_LINUX
