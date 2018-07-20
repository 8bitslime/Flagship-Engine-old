#include "arena.h"
#include <stdlib.h>

arena_t arena_default = {NULL, 0, 0};

void arena_alloc(arena_t *arena, size_t size) {
	byte_t *data = malloc(size);
	
	if (data == NULL) {
		FATAL_ERROR("malloc failed!");
		return;
	}
	
	arena->data = data;
	arena->cap = size;
	arena->size = 0;
}
void arena_free(arena_t *arena) {
	free(arena->data);
	arena->cap = 0;
	arena->size = 0;
}

void *arena_getSpace(arena_t *arena, size_t size) {
	if (arena == NULL) {
		arena = &arena_default;
	}
	
	if (arena->size + size <= arena->cap) {
		void *out = arena->data + arena->size;
		arena->size += size;
		return out;
	} else {
		return NULL;
	}
}
void arena_empty(arena_t *arena) {
	arena->size = 0;
}
