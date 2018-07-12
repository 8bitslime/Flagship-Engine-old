#include "text.h"
#include "texture.h"
#include "shader.h"
#include <glad/glad.h>
#include <cglm/cglm.h>

#include <stdio.h>
#include <stdlib.h>

typedef struct {
	vec2 character;
	vec2 position;
} character_t;

static GLuint text_vao;
static GLuint text_vbo[2];

static program_t text_program;
static uniform_t text_uniform_matrix;
static uniform_t text_uniform_char;
static uniform_t text_uniform_pos;

int text_init(void) {
	//TODO: better file loading
	char shader[1024];
	FILE *file = fopen("res/text.glsl", "rb");
	fseek(file, 0, FILE_END);
	GLuint length = ftell(file);
	rewind(file);
	if (file == NULL) {
		printf("failed to load file: res/text.glsl\n");
	}
	fread(shader, sizeof(char), length, file);
	shader[length] = '\0';
	
	text_program = program_create(shader);
	
	glGenVertexArrays(1, &text_vao);
	glBindVertexArray(text_vao);
	
	glGenBuffers(2, text_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*12, (float[]){
		0, 1,
		1, 0,
		0, 0,
		0, 1,
		1, 1,
		1, 0
	}, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, NULL);
	
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(character_t)*128, NULL, GL_STREAM_DRAW);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(character_t), (void*)0);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(character_t), (void*)sizeof(vec2));
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	
	text_uniform_matrix = glGetUniformLocation(text_program, "ortho");
	text_uniform_char   = glGetUniformLocation(text_program, "character");
	text_uniform_pos    = glGetUniformLocation(text_program, "position");
	
	return 0;
}
void text_free(void) {
	glDeleteProgram(text_program);
	glBindVertexArray(text_vao);
	glDeleteBuffers(2, text_vbo);
	glDeleteVertexArrays(1, &text_vao);
}

void text_bind(GLuint texture) {
	glUseProgram(text_program);
	glBindVertexArray(text_vao);
	glBindBuffer(GL_ARRAY_BUFFER, text_vbo[1]);
	texture_bind(texture, 0);
	
	mat4 ortho;
	glm_ortho(0, 650, 450, 0, -1, 1, ortho);
	glUniformMatrix4fv(text_uniform_matrix, 1, GL_FALSE, (float*)ortho);
}
void text_draw(const char *string, float x, float y) {
	vec2 cursor = {0, 0};
	character_t buffer[128];
	
	const char *c = string;
	while (*c != '\0') {
		int i;
		for (i = 0; *c && (i < 128); c++, i++) {
			if (*c == '\n') {
				cursor[0] = 0;
				cursor[1]++;
				continue;
			}
			
			float col = (float)(*c % 16);
			float row = (float)(*c / 16);
			
			buffer[i] = (character_t){
				{col, row},
				{cursor[0] * 16, cursor[1] * 16}
			};
			
			cursor[0]++;
		}
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(character_t)*i, buffer);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, i);
	}
}
