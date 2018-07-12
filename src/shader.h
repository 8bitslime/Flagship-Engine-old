#pragma once
#include <glad/glad.h>

typedef GLuint shader_t;
typedef GLuint program_t;
typedef GLint uniform_t;


// Create a program from a shader string
program_t program_create(const char *shader);