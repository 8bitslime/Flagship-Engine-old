#include "text.h"
#include "texture.h"
#include "cvar.h"
#include "cmd.h"
#include "console.h"
#include "mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>

cvar_t hello = {"hello", "13"};

void input_char(GLFWwindow *window, unsigned int c) {
	con_input(c);
}

void input_key(GLFWwindow *window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		switch (key) {
			case GLFW_KEY_BACKSPACE: con_input('\b'); break;
			case GLFW_KEY_ENTER:     con_input('\n'); break;
			case GLFW_KEY_PAGE_UP:   con_seekUp();    break;
			case GLFW_KEY_PAGE_DOWN: con_seekDown();  break;
		}
	}
}

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main() {
	
	cmd_init();
	// cvar_register(&hello);
	
	// while (1) {
	// 	char buffer[1024];
	// 	gets_s(buffer, 1024);
	// 	cmd_exec(buffer);
	// }
	
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(650, 450, "Flagship Engine", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	glfwSetWindowSizeCallback(window, resize);
	glfwSetCharCallback(window, input_char);
	glfwSetKeyCallback(window, input_key);
	
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	texture_t t = texture_load("res/font.bmp");
	
	con_init();
	text_init();
	
	mesh_t mesh;
	mesh_createStatic(&mesh, 2);
	mesh_floatBufferData(&mesh, 0, 3, 3, (float[]){
		0, 1, 0,
		1, -1, 0,
		-1, -1, 0
	});
	
	mesh_elementBufferData(&mesh, 1, 3, (unsigned int[]) {
		0, 1, 2
	});
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(0);
		mesh_draw(&mesh, 3, 0);
		
		text_bind(t);
		con_draw(0, 0);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}