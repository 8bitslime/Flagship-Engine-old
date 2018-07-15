#include "mesh.h"

void mesh_createStatic(mesh_t *dest, int numBuffers) {
	glGenVertexArrays(1, &dest->vao);
	glBindVertexArray(dest->vao);
	
	glGenBuffers(numBuffers, dest->vbo);
	dest->numBuffers = numBuffers;
	
	dest->flags = MESH_STATIC_DRAW;
}
void mesh_free(mesh_t *mesh) {
	glDeleteVertexArrays(1, &mesh->vao);
	glDeleteBuffers(mesh->numBuffers, mesh->vbo);
}

void mesh_floatBufferData(mesh_t *mesh, int buffer, int type, int size, const float *data) {
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo[buffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * type * size, data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(buffer);
	glVertexAttribPointer(buffer, type, GL_FLOAT, GL_FALSE, sizeof(float) * type, 0);
}

void mesh_elementBufferData(mesh_t *mesh, int buffer, int size, const unsigned int *data) {
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->vbo[buffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * size, data, GL_STATIC_DRAW);
	mesh->flags |= MESH_INDEXED;
}

void mesh_draw(mesh_t *mesh, int count, int offset) {
	glBindVertexArray(mesh->vao);
	if (mesh->flags & MESH_INDEXED) {
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(unsigned int)));
	} else {
		glDrawArrays(GL_TRIANGLES, offset, count);
	}
}