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

#define MAX_CONSOLE_BUFFER 8192
static buffer_t console_buffer;

#define MAX_LINES 30
#define MAX_LINE_LENGTH 128
static int console_backtrack = 0;
static char console_lines[MAX_LINES][MAX_LINE_LENGTH];
static int viewable_lines = MAX_LINES;

#define MAX_COMMAND_HISTORY 30
static int command_history_backtrack = 0;
static int command_history_end = 0;
static char command_history_buffer[MAX_COMMAND_HISTORY][MAX_LINE_LENGTH];

static int  input_cursor = 0;
static int  input_length = 0;

static void cmd_clear_f(void) {
	buffer_empty(&console_buffer);
}

static void cmd_cld_f(void) {
	for (int i = 0; i < viewable_lines; i++) {
		con_print("\n");
	}
}

static cvar_t echo = {"echo", "0", CVAR_INTEGER};

static cmd_t clear = {"clear", cmd_clear_f};
static cmd_t cls   = {"cls", cmd_cld_f};

void con_init(void) {
	cvar_register(&echo);
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
	
	char *input_buffer = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	
	if (sin(glfwGetTime() * 4) > 0 ) {
		text_draw("\xDD", (float)input_cursor + 2, (float)viewable_lines);
	}
	text_draw("\xAF", 0, (float)viewable_lines);
	text_draw(input_buffer, 2, (float)viewable_lines);
}

static void con_exec_input(void) {
	char *input_buffer = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	
	if (echo.ival) {
		con_printf("?\xAF %s\n", input_buffer);
	}
	
	if (strlen(input_buffer) == 0) {
		return;
	}
	
	cmd_addString(input_buffer);
	input_cursor = 0;
	input_length = 0;
	
	int current = command_history_end % MAX_COMMAND_HISTORY;
	
	if (command_history_backtrack < command_history_end - 1) {
		memcpy(command_history_buffer[current], input_buffer, MAX_LINE_LENGTH);
		command_history_end++;
	} else if (command_history_backtrack == command_history_end - 1) {
		memset(command_history_buffer[current], 0, MAX_LINE_LENGTH);
	} else {
		int history_next = ++command_history_end % MAX_COMMAND_HISTORY;
		memset(command_history_buffer[history_next], 0, MAX_LINE_LENGTH);
	}
	
	command_history_backtrack = command_history_end;
}

void con_input(char c) {
	console_backtrack = 0;
	char *input_buffer = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	
	char temp[MAX_LINE_LENGTH];
	memcpy(temp, input_buffer + input_cursor, max(input_length - input_cursor, 1));
	
	if (c == '\n') {
		con_exec_input();
		return;
	} else if (c == '\b') {
		if (input_cursor && input_length) {
			memcpy(input_buffer + input_cursor - 1, temp, input_length - input_cursor + 1);
			input_cursor--;
			input_length--;
		}
		return;
	}
	
	if (input_length >= MAX_LINE_LENGTH) {
		return;
	}
	
	memcpy(input_buffer + input_cursor + 1, temp, input_length - input_cursor);
	input_buffer[input_cursor++] = c;
	input_length++;
}

void con_historyUp(void) {
	command_history_backtrack = max(command_history_backtrack - 1, 0);
	
	int end = command_history_end % MAX_COMMAND_HISTORY;
	int back = command_history_backtrack % MAX_COMMAND_HISTORY;
	
	if (back == end) {
		command_history_backtrack++;
	}
	
	char *input_buffer = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	input_length = strlen(input_buffer);
	input_cursor = input_length;
}

void con_historyDown(void) {
	command_history_backtrack = min(command_history_backtrack + 1, command_history_end);
	char *input_buffer = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	input_length = strlen(input_buffer);
	input_cursor = input_length;
}

void con_cursorLeft(void) {
	input_cursor = max(input_cursor - 1, 0);
}

void con_cursorRight(void) {
	input_cursor = min(input_cursor + 1, input_length);
}

void con_cursorHome(void) {
	input_cursor = 0;
}

void con_cursorEnd(void) {
	input_cursor = input_length;
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
