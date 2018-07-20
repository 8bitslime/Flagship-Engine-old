#pragma once
#include "flagship.h"
#include "arena.h"

typedef struct buffer_t {
	byte_t *data;
	size_t cap, size;
} buffer_t;

void buffer_alloc(buffer_t *dest, size_t size, arena_t *allocator);
// void buffer_free(buffer_t *buffer);

void *buffer_getSpace(buffer_t *buffer, size_t size);
void buffer_empty(buffer_t *buffer);
