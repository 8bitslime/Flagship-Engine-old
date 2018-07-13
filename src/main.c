#include "text.h"
#include "texture.h"
#include "cvar.h"
#include <stdio.h>
#include <GLFW/glfw3.h>

cvar_t hello = {"hello", "13"};

void resize(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
}

int main() {
	
	cvar_register(&hello);
	cvar_set("hello", "107");
	cvar_setf("hello", 7);
	cvar_t *thing = cvar_find("hello");
	printf("cvar value: %f %s\n", thing->fval, thing->value);
	
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
		text_draw("THANKS FOR WATCHING\n\nText rendering [CHECK]\nInstacned text [CHECK]\nCvar           [CHECK]\n\nTomorrow:\n dev console\n text input", 10, 8);
		
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}