#pragma once
#include <glad/glad.h>

typedef GLuint texture_t;

texture_t texture_load(const char *fileName);
void   texture_free(GLuint texture);

void texture_bind(GLuint texture, int location);