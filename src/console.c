#include "console.h"
#include "flagship.h"
#include "cvar.h"
#include "cmd.h"
#include "text.h"
#include "buffer.h"
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

#define MAX_INPUT_BUFFER 255
static int  input_cursor = 0;
static char input_buffer[MAX_INPUT_BUFFER + 1] = {0};

#define MAX_CONSOLE_BUFFER 8192
static buffer_t console_buffer;

#define MAX_LINES 30
#define MAX_LINE_LENGTH 128
static int console_backtrack = 0;
static char console_lines[MAX_LINES][MAX_LINE_LENGTH];
static int viewable_lines = MAX_LINES;

static void con_clear(void) {
	buffer_empty(&console_buffer);
}

static void con_cls(void) {
	for (int i = 0; i < viewable_lines; i++) {
		con_print("\n");
	}
}

static cmd_t clear = {"clear", con_clear};
static cmd_t cls   = {"cls", con_cls};

void con_init(void) {
	cmd_register(&clear);
	cmd_register(&cls);
	buffer_alloc(&console_buffer, MAX_CONSOLE_BUFFER);
}

void con_draw(int w, int h) {
	viewable_lines = min(h / 38, MAX_LINES);
	
	for (int i = 0; i < MAX_LINES; i++) {
		memset(console_lines[i], 0, MAX_LINE_LENGTH);
	}
	
	char *buffer = (char*)console_buffer.data;
	
	int backtrack = console_backtrack;
	int size = console_buffer.size-1;
	if (buffer[size] == '\n') {
		size--;
	}
	
	int line = viewable_lines-1;
	for (int i = size, end = size; line >= 0; i--) {
		if (i <= 0) {
			memcpy(console_lines[line], buffer, end + 1);
			console_lines[line][end + 1] = '\0';
			console_backtrack -= backtrack;
			break;
		} else if (buffer[i] == '\n') {
			int length = end - i;
			end = i - 1;
			
			if (backtrack) {
				backtrack--;
				continue;
			}
			
			if (!length) {
				line--;
				continue;
			} else if (length > MAX_LINE_LENGTH - 2) {
				length = MAX_LINE_LENGTH - 2;
			}
			
			memcpy(console_lines[line], buffer + i + 1, length);
			console_lines[line][length] = '\0';
			line--;
		}
	}
	
	text_setColor(1, 1, 1);
	for (int i = 0; i < viewable_lines; i++) {
		char *line = console_lines[i];
		switch (*line) {
			case '!' : text_setColor(1, 0, 0); line++; break;
			case '?' : text_setColor(0.6f, 0.6f, 0.6f); line++; break;
		}
		text_draw(line, 0, (float)i);
		text_setColor(1, 1, 1);
	}
	
	
	if (sin(glfwGetTime() * 4) > 0 ) {
		text_draw("\xDD", (float)strlen(input_buffer) + 1.8f, (float)viewable_lines);
	}
	text_draw("\xAF", 0, (float)viewable_lines);
	text_draw(input_buffer, 1.5f, (float)viewable_lines);
}

static void con_exec_input(void) {
	cmd_addString(input_buffer);
	input_cursor = 0;
	memset(input_buffer, 0, MAX_INPUT_BUFFER);
}

void con_input(char c) {
	console_backtrack = 0;
	
	if (c == '\n') {
		con_exec_input();
		return;
	} else if (c == '\b') {
		if (input_cursor) {
			input_buffer[--input_cursor] = '\0';
		}
		return;
	}
	
	if (input_cursor >= MAX_INPUT_BUFFER) {
		return;
	}
	
	input_buffer[input_cursor++] = c;
}

void con_print(const char *string) {
	size_t length = strlen(string);
	char *buffer = buffer_getSpace(&console_buffer, length);
	
	if (buffer == NULL) {
		//TODO: dump console buffer to file
		buffer_empty(&console_buffer);
		con_print("!console buffer overflow!\n");
		return;
	}
	
	memcpy(buffer, string, length);
}

void con_printf(const char *format, ...) {
	char buffer[1024];
	
	va_list list;
	va_start(list, format);
	vsnprintf(buffer, 1024, format, list);
	va_end(list);
	
	con_print(buffer);
}

void con_seekUp(void) {
	console_backtrack++;
}

void con_seekDown(void) {
	console_backtrack--;
	if (console_backtrack < 0) {
		console_backtrack = 0;
	}
}

char *con_buffer(void) {
	return (char*)console_buffer.data;
}
