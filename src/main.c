#include "text.h"
#include "texture.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main () {
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(650, 450, "Flagship Engine", NULL, NULL);
	glfwMakeContextCurrent(window);
	
	glfwSetWindowSizeCallback(window, resize);
	
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glClearColor(0, 0.3f, 1.0f, 1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	texture_t t = texture_load("res/font.bmp");
	// printf("%u\n", t);
	
	text_init();
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		
		text_bind(t);
		text_draw("Hello world\nHELLO WORLD!", 0, 0);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}