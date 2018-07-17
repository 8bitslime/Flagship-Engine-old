#pragma once
#include "flagship.h"
#include <GLFW/glfw3.h>

void glfw_keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_charHandler(GLFWwindow *window, unsigned int character);

void input_key(int key, int action, int mods);
void input_char(unsigned int c);
