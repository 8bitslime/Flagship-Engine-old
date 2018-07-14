#include "text.h"
#include "texture.h"
#include "cvar.h"
#include "cmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLFW/glfw3.h>

cvar_t hello = {"hello", "13"};

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main() {
	
	// cmd_init();
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
	
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glClearColor(0, 0.3f, 1.0f, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	texture_t t = texture_load("res/font.bmp");
	
	text_init();
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		
		text_bind(t);
		text_draw("Feeding my doggy! :)", 10, 8);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}