#include "flagship.h"
#include "arena.h"
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

//TEMP
#define ZAC_GL_UTIL
#include "zac.h"

static cvar_t fov = {"fov", "90"};

int main() {
	//initialize default arena
	arena_alloc(&arena_default, 1024 * 1024);
	
	cmd_init();
	con_init();
	cvar_init();
	
	cvar_register(&fov);
	con_printf("Flagship Engine dev build\n");
	
	if (window_create("Flagship Engine", 650, 450) != 0) {
		exit(EXIT_FAILURE);
	}
	
	char thing[1024];
	FILE *file = fopen("res/def.glsl", "rb");
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
	
	texture_t t = texture_load("res/terminal16x16.png");
	
	zac_gl_obj *obj = zac_gl_parseObj("res/monkey.obj");
	
	con_printf("object name: %s\n", obj->name);
	con_printf("vertices: %i\n", zac_array_len(obj->vertices));
	
	mesh_t mesh;
	mesh_createStatic(&mesh, 3);
	int verts = zac_array_len(obj->vertices);
	mesh_floatBufferData(&mesh, 0, 3, verts, (float*)obj->vertices);
	mesh_floatBufferData(&mesh, 1, 3, verts, (float*)obj->normals);
	mesh_floatBufferData(&mesh, 2, 2, verts, (float*)obj->uvs);
	
	zac_gl_freeObj(obj);
	
	while (!window_shouldClose()) {
		window_pollEvents();
		cmd_execBuffer();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		texture_bind(t, 0);
		
		glUseProgram(shader);
		
		versor quat;
		glm_quat(quat, glm_rad(sin(glfwGetTime()) * 180), 0, 1, 0);
		mat4 rotate;
		glm_quat_mat4(quat, rotate);
		mat4 matrix;
		mat4 translate = {
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, -5, 1 }
		};
		glm_mat4_mul(translate, rotate, translate);
		glm_perspective(glm_rad(fov.fval), (float)window_width / (float)window_height, 0.01f, 1000.f, matrix);
		glm_mat4_mul(matrix, translate, matrix);
		glUniformMatrix4fv(per_mat, 1, GL_FALSE, (float*)matrix);
		
		mesh_draw(&mesh, verts, 0);
		
		if (console_open) {
			text_begin(t);
			con_draw();
			glEnable(GL_DEPTH_TEST);
		}
		
		window_swapBuffers();
	}
	mesh_free(&mesh);
	window_free();
	
	arena_free(&arena_default);
	return 0;
}
