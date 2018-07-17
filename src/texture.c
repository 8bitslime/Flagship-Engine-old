#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

texture_t missing_texture = 0;

texture_t texture_load(const char *fileName) {
	int w,h,c;
	stbi_uc *image = stbi_load(fileName, &w, &h, &c, 3);
	
	if (image == NULL) {
		return missing_texture;
	}
	
	texture_t out;
	glGenTextures(1, &out);
	glBindTexture(GL_TEXTURE_2D, out);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	stbi_image_free(image);
	
	return out;
}
void texture_free(GLuint texture) {
	glDeleteTextures(1, &texture);
}

void texture_bind(GLuint texture, int location) {
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture(GL_TEXTURE_2D, texture);
}
