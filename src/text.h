#pragma once
#include "texture.h"

int text_init(void);
void text_free(void);

void text_begin(texture_t texture, int w, int h);
void text_draw(const char *string, float x, float y);
void text_setColor(float r, float g, float b);
