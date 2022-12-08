#include "es_header.h"

#include <malloc.h>
#include <string.h>

/*=========================*/
// Dynamic array
/*=========================*/

void _es_da_init_impl(void **arr, usize_t size) {
	if (*arr != NULL) {
		return;
	}

	_es_da_header_t *head = es_malloc(sizeof(_es_da_header_t) + size * _ES_DA_INIT_CAP);

	head->cap = _ES_DA_INIT_CAP;
	head->size = size;
	head->count = 0;

	*arr = _es_da_ptr(head);
}

void _es_da_insert_impl(void **arr, const void *data, usize_t index) {
	_es_da_resize(arr, true);
	_es_da_header_t *head = _es_da_head(*arr);

	u8_t *ptr = *arr;
	void *src = ptr + (index) * head->size;
	void *dest = ptr + (index + 1) * head->size;

	memcpy(dest, src, (head->count - index) * head->size);
	memcpy(src, data, head->size);
	head->count++;
}

void _es_da_remove_impl(void **arr, usize_t index, void *output) {
	_es_da_resize(arr, false);
	_es_da_header_t *head = _es_da_head(*arr);

	u8_t *ptr = *arr;
	void *src = ptr + (index + 1) * head->size;
	void *dest = ptr + (index) * head->size;

	if (output) {
		memcpy(output, dest, head->size);
	}

	memcpy(dest, src, (head->count - index - 1) * head->size);
	head->count--;
}

void _es_da_insert_fast_impl(void **arr, const void *data, usize_t index) {
	_es_da_resize(arr, true);
	_es_da_header_t *head = _es_da_head(*arr);

	u8_t *ptr = *arr;
	void *src = ptr + (index) * head->size;
	void *dest = ptr + head->count * head->size;

	memcpy(dest, src, head->size);
	memcpy(src, data, head->size);
	head->count++;
}

void _es_da_remove_fast_impl(void **arr, usize_t index, void *output) {
	_es_da_resize(arr, false);
	_es_da_header_t *head = _es_da_head(*arr);

	u8_t *ptr = *arr;
	void *src = ptr + (head->count - 1) * head->size;
	void *dest = ptr + (index) * head->size;

	if (output) {
		memcpy(output, dest, head->size);
	}

	memcpy(dest, src, head->size);
	head->count--;
}

void _es_da_free_impl(void **arr) {
	es_free(_es_da_head(*arr));
}

void _es_da_resize(void **arr, b8_t insert) {
	_es_da_header_t *head = _es_da_head(*arr);
	// Double the capacity when full and inserting
	if (insert && head->count + 1 == head->cap) {
		head->cap <<= 1;
	}
	// Half capacity when half empty and removing
	else if (!insert && head->count - 1 == head->cap >> 1) {
		head->cap >>= 1;
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
