#pragma once
#include "flagship.h"

typedef struct arena_t {
	byte_t *data;
	size_t cap, size;
} arena_t;

extern arena_t arena_default;

void arena_alloc(arena_t *arena, size_t size);
void arena_free(arena_t *arena);

void *arena_getSpace(arena_t *arena, size_t size);
void arena_empty(arena_t *arena);
