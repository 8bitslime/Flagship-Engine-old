#pragma once
#include <glad/glad.h>

int text_init(void);
void text_free(void);

void text_bind(GLuint texture);
void text_draw(const char *string, float x, float y);