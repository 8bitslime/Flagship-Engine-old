#pragma once
#include "flagship.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

extern GLFWwindow *window;
extern int window_width;
extern int window_height;

#define window_pollEvents glfwPollEvents
#define window_swapBuffers() glfwSwapBuffers(window)
#define window_shouldClose() glfwWindowShouldClose(window)

int window_create(const char *title, int w, int h);
void window_free(void);

void window_toggleFullscreen(void);
