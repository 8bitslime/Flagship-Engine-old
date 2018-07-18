#include "window.h"
#include "input.h"
#include "cmd.h"
#include "console.h"
#include "text.h"
#include <stdio.h>

GLFWwindow *window;
int window_width;
int window_height;

static bool_t fullscreen = false;
static cmd_t fullscreen_cmd = {"fullscreen", window_toggleFullscreen};

static void resize(GLFWwindow *window, int w, int h) {
	window_width = w;
	window_height = h;
	glViewport(0, 0, w, h);
}

int window_create(const char *title, int w, int h) {
	if (glfwInit() == GLFW_FALSE) {
		printf("GLFW failed to initialize!\n");
		return -1;
	}
	
	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	
	window = glfwCreateWindow(w, h, title, NULL, NULL);
	window_width = w;
	window_height = h;
	
	if (window == NULL) {
		printf("GLFW failed to create a window!\n");
		glfwTerminate();
		return -1;
	}
	
	glfwSwapInterval(GLFW_FALSE);
	
	glfwSetWindowSizeCallback(window, resize);
	glfwSetKeyCallback(window, glfw_keyHandler);
	glfwSetCharCallback(window, glfw_charHandler);
	
	glfwShowWindow(window);
	glfwMakeContextCurrent(window);
	
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	
	//set up OpenGL
	//TODO: make this a seperate function
	glEnable(GL_DEPTH_TEST);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	
	con_printf("GL driver: %s\n", glGetString(GL_RENDERER));
	
	if (text_init() != 0) {
		printf("text renderer failed to initialize!");
		glfwTerminate();
		return 0;
	}
	
	cmd_register(&fullscreen_cmd);
	
	return 0;
}

void window_free(void) {
	text_free();
	
	glfwDestroyWindow(window);
	glfwTerminate();
}

void window_toggleFullscreen(void) {
	static int x, y, w, h;
	if (fullscreen) {
		glfwSetWindowMonitor(window, NULL, x, y, w, h, GLFW_DONT_CARE);
		fullscreen = false;
	} else {
		glfwGetWindowPos(window, &x, &y);
		glfwGetWindowSize(window, &w, &h);
		const GLFWvidmode *temp = glfwGetVideoMode(glfwGetPrimaryMonitor());
		glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, temp->width, temp->height, GLFW_DONT_CARE);
		fullscreen = true;
	}
}
