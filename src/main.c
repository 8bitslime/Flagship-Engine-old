#include "flagship.h"
#include "cvar.h"
#include "cmd.h"
#include "console.h"
#include "window.h"
#include "text.h"
#include "mesh.h"
#include "shader.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cglm/cglm.h>

static cvar_t fov = {"fov", "90"};

int main() {
	cmd_init();
	con_init();
	cvar_init();
	
	cvar_register(&fov);
	con_printf("Flagship Engine dev build\n");
	
	if (window_create("Flagship Engine", 650, 450) != 0) {
		exit(EXIT_FAILURE);
	}
	
	char thing[1024];
	FILE *file = fopen("res/def.glsl", "r");
	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	rewind(file);
	fread(thing, sizeof(char), length, file);
	thing[length] = '\0';
	fclose(file);
	
	program_t shader = program_create(thing);
	glUseProgram(shader);
	uniform_t per_mat = glGetUniformLocation(shader, "perspective");
	// con_printf("per_mat: %i\n", per_mat);
	
	mesh_t mesh;
	mesh_createStatic(&mesh, 2);
	mesh_floatBufferData(&mesh, 0, 3, 4, (float[]){
		-1, -1, -5.f,
		-1,  1, -5.f,
		 1,  1, -5.f,
		 1, -1, -5.f
	});
	
	mesh_elementBufferData(&mesh, 1, 6, (unsigned int[]) {
		2, 1, 0, 2, 0, 3
	});
	
	texture_t t = texture_load("res/terminal16x16.png");
	
	while (!window_shouldClose()) {
		window_pollEvents();
		cmd_execBuffer();
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shader);
		mat4 matrix;
		glm_perspective(glm_rad(fov.fval), (float)window_width / (float)window_height, 0.01f, 1000.f, matrix);
		glUniformMatrix4fv(per_mat, 1, GL_FALSE, (float*)matrix);
		
		mesh_draw(&mesh, 6, 0);
		
		if (console_open) {
			text_begin(t);
			con_draw();
		}
		
		window_swapBuffers();
	}
	mesh_free(&mesh);
	window_free();
	return 0;
}
