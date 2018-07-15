#pragma once
#include <stdlib.h>

typedef unsigned char byte_t;

typedef struct buffer_t {
	byte_t *data;
	size_t cap, size;
} buffer_t;

void buffer_alloc(buffer_t *dest, size_t size);
void buffer_free(buffer_t *buffer);

void *buffer_getSpace(buffer_t *buffer, size_t size);
void buffer_empty(buffer_t *buffer);