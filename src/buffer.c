#include "buffer.h"
#include <memory.h>

void buffer_alloc(buffer_t *dest, size_t size) {
	void *data = malloc(size);
	if (data == NULL) {
		dest->data = NULL;
		dest->cap = 0;
		dest->size = 0;
		return;
	}
	dest->data = data;
	dest->cap = size;
	dest->size = 0;
}
void buffer_free(buffer_t *buffer) {
	free(buffer->data);
	buffer->cap = 0;
	buffer->size = 0;
}

void *buffer_getSpace(buffer_t *buffer, size_t size) {
	if (buffer->size + size <= buffer->cap) {
		void *out = buffer->data + buffer->size;
		buffer->size += size;
		return out;
	} else {
		return NULL;
	}
}
void buffer_empty(buffer_t *buffer) {
	// memset(buffer->data, 0, buffer->cap);
	buffer->size = 0;
}
