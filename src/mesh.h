#pragma once
#include <glad/glad.h>

#define MESH_MAX_BUFFERS 8

enum {
	MESH_STATIC_DRAW = 0,
	MESH_INDEXED = 1 << 0
};

typedef struct mesh_t {
	GLuint vao;
	GLuint vbo[MESH_MAX_BUFFERS];
	int numBuffers;
	int flags;
} mesh_t;

void mesh_createStatic(mesh_t *dest, int numBuffers);
void mesh_free(mesh_t *mesh);

void mesh_floatBufferData(mesh_t *mesh, int buffer, int type, int size, const float *data);
void mesh_elementBufferData(mesh_t *mesh, int buffer, int size, const unsigned int *data);

void mesh_draw(mesh_t *mesh, int count, int offset);