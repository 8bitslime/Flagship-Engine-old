#include "shader.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>

static GLboolean compile_status(shader_t shader) {
	glCompileShader(shader);
	
	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	
	if (status == GL_FALSE) {
		//compilation failure
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		
		char *buffer = malloc(length + 1);
		glGetShaderInfoLog(shader, length, &length, buffer);
		
		con_printf("!shader compilation error:\n%s", buffer);
		
		free(buffer);
		return GL_FALSE;
	}
	
	return GL_TRUE;
}

static GLboolean link_status(program_t program) {
	glLinkProgram(program);
	
	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		
		char *buffer = malloc(length + 1);
		glGetProgramInfoLog(program, length, &length, buffer);
		
		printf("program link error: %s", buffer);
		
		free(buffer);
		return GL_FALSE;
	}
	
	return GL_TRUE;
}

program_t program_create(const char *shader) {
	shader_t vert = glCreateShader(GL_VERTEX_SHADER);
	const char *vertShader[] = {"#version 330 core\n #define VERTEX\n", shader};
	glShaderSource(vert, 2, vertShader, NULL);
	
	if (compile_status(vert) == GL_FALSE) {
		//abort
		glDeleteShader(vert);
		return 0;
	}
	
	shader_t frag = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragShader[] = {"#version 330 core\n #define FRAGMENT\n", shader};
	glShaderSource(frag, 2, fragShader, NULL);
	
	if (compile_status(frag) == GL_FALSE) {
		//abort
		glDeleteShader(vert);
		glDeleteShader(frag);
		return 0;
	}
	
	program_t out = glCreateProgram();
	
	glAttachShader(out, vert);
	glAttachShader(out, frag);
	
	if (link_status(out) == GL_FALSE) {
		//abort
		glDeleteProgram(out);
		glDeleteShader(vert);
		glDeleteShader(frag);
		return 0;
	}
	
	glDetachShader(out, vert);
	glDetachShader(out, frag);
	glDeleteShader(vert);
	glDeleteShader(frag);
	
	return out;
}
