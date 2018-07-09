#include <stdio.h>
#include <GLFW/glfw3.h>

int main () {
	printf("hello world!\n");
	
	glfwInit();
	GLFWwindow *window = glfwCreateWindow(650, 450, "Flagship", NULL, NULL);
	glfwMakeContextCurrent(window);
	glClearColor(0, 0.3f, 1.0f, 1);
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}
	
	glfwTerminate();
	return 0;
}