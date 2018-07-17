#include "input.h"
#include "console.h"

void glfw_keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods) {
	input_key(key, action, mods);
}

void glfw_charHandler(GLFWwindow *window, unsigned int character) {
	input_char(character);
}


void input_key(int key, int action, int mods) {
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		con_toggle();
		return;
	}
	
	if (console_open) {
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			switch (key) {
				case GLFW_KEY_BACKSPACE: con_input('\b');   break;
				case GLFW_KEY_ENTER:     con_input('\n');   break;
				case GLFW_KEY_PAGE_UP:   con_seekUp();      break;
				case GLFW_KEY_PAGE_DOWN: con_seekDown();    break;
				case GLFW_KEY_LEFT:      con_cursorLeft();  break;
				case GLFW_KEY_RIGHT:     con_cursorRight(); break;
				case GLFW_KEY_UP:        con_historyUp();   break;
				case GLFW_KEY_DOWN:      con_historyDown(); break;
				case GLFW_KEY_HOME:      con_cursorHome();  break;
				case GLFW_KEY_END:       con_cursorEnd();   break;
			}
		}
	} else {
		//game input
		//TODO: hire someone else to code this engine
	}
}

void input_char(unsigned int character) {
	//TODO: text input handler
	if (console_open) {
		if (character != '~' && character != '`') {
			con_input(character);
		}
	}
}
