#include "console.h"
#include "cvar.h"
#include "cmd.h"
#include "window.h"
#include "text.h"
#include "buffer.h"
#include <math.h>
#include <memory.h>
#include <stdarg.h>
#include <stdio.h>
#include <cglm/cglm.h>

#define MAX_CONSOLE_BUFFER 8192
static buffer_t console_buffer = {NULL, 0, 0};

#define MAX_LINES 30
#define MAX_LINE_LENGTH 128
static int console_backtrack = 0;
static char console_lines[MAX_LINES][MAX_LINE_LENGTH];
static int viewable_lines = MAX_LINES;

#define MAX_COMMAND_HISTORY 30
static int command_history_backtrack = 0;
static int command_history_end = 0;
static char command_history_buffer[MAX_COMMAND_HISTORY][MAX_LINE_LENGTH];

static int input_cursor = 0;
static int input_length = 0;

bool_t console_open;
double console_time;

void con_toggle(void) {
	console_open = !console_open;
	console_time = glfwGetTime();
}

static void cmd_clear_f(void) {
	buffer_empty(&console_buffer);
}

static void cmd_cld_f(void) {
	for (int i = 0; i < viewable_lines; i++) {
		con_print("\n");
	}
}

static float r = 1, g = 1, b = 1;

static void cmd_color_f(void) {
	if (cmd_argc == 1) {
		r = g = b = 1;
	} else if (cmd_argc == 4) {
		r = strtof(cmd_argv[1], NULL);
		g = strtof(cmd_argv[2], NULL);
		b = strtof(cmd_argv[3], NULL);
	} else {
		con_printf("?color <r> <g> <b>\n");
	}
}

static cvar_t echo = {"echo", "0", CVAR_INTEGER};

static cmd_t clear = {"clear", cmd_clear_f};
static cmd_t cls   = {"cls", cmd_cld_f};
static cmd_t color = {"color", cmd_color_f};

void con_init(void) {
	buffer_alloc(&console_buffer, MAX_CONSOLE_BUFFER, NULL);
	
	if (console_buffer.cap != MAX_CONSOLE_BUFFER) {
		FATAL_ERROR("could not allocate console buffer");
		return;
	}
	
	cvar_register(&echo);
	cmd_register(&clear);
	cmd_register(&cls);
	cmd_register(&color);
}

void con_draw() {
	viewable_lines = min(window_height / 38, MAX_LINES);
	
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
			console_backtrack -= backtrack;
			if (*buffer != '\xAF' || echo.ival) {
				memcpy(console_lines[line], buffer, end + 1);
				console_lines[line][end + 1] = '\0';
			}
			break;
		} else if (buffer[i] == '\n') {
			int length = end - i;
			end = i - 1;
			
			if (*(buffer + i + 1) == '\xAF' && !echo.ival) {
				continue;
			}
			
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
	
	float curTime = glfwGetTime() - console_time;
	curTime = max(viewable_lines + 1 - (glm_ease_bounce_out(curTime * 1) * (viewable_lines + 1)), 0);
	
	text_setColor(r, g, b);
	for (int i = 0; i < viewable_lines; i++) {
		char *line = console_lines[i];
		switch (*line) {
			case '!'    : text_setColor(1, 0, 0); line++; break;
			case '\xAF' : line--;
			case '?'    : text_setColor(0.6f, 0.6f, 0.6f); line++; break;
		}
		text_draw(line, 0, (float)i - curTime);
		text_setColor(r, g, b);
	}
	
	char *input_buffer = command_history_buffer[command_history_end % MAX_COMMAND_HISTORY];
	
	if (sin(glfwGetTime() * 5) > 0 ) {
		text_draw("\xDD", (float)input_cursor + 2.2f, (float)viewable_lines - curTime);
	}
	text_draw("\xAF", 0, (float)viewable_lines - curTime);
	text_draw(input_buffer, 2, (float)viewable_lines - curTime);
}

static void con_exec_input(void) {
	console_backtrack = 0;
	char *input_buffer = command_history_buffer[command_history_end % MAX_COMMAND_HISTORY];
	con_printf("\xAF %s\n", input_buffer);
	
	if (strlen(input_buffer) == 0) {
		return;
	}
	
	cmd_addString(input_buffer);
	input_cursor = 0;
	input_length = 0;
	
	int last = (command_history_end - 1) % MAX_COMMAND_HISTORY;
	int current = command_history_backtrack % MAX_COMMAND_HISTORY;
	char *previous = command_history_buffer[(command_history_end - 1) % MAX_COMMAND_HISTORY];
	
	if (strcmp(input_buffer, previous) == 0 && current == last) {
		memset(input_buffer, 0, MAX_LINE_LENGTH);
	} else {
		int next = ++command_history_end % MAX_COMMAND_HISTORY;
		memset(command_history_buffer[next], 0, MAX_LINE_LENGTH);
	}
	
	command_history_backtrack = command_history_end;
}

void con_input(char c) {
	char *input_buffer = command_history_buffer[command_history_end % MAX_COMMAND_HISTORY];
	
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
	
	char *input_buffer = command_history_buffer[end];
	char *history_select = command_history_buffer[back];
	
	memcpy(input_buffer, history_select, MAX_LINE_LENGTH);
	
	input_length = strlen(input_buffer);
	input_cursor = input_length;
}

void con_historyDown(void) {
	command_history_backtrack = min(command_history_backtrack + 1, command_history_end);
	
	char *history_select = command_history_buffer[command_history_backtrack % MAX_COMMAND_HISTORY];
	char *input_buffer = command_history_buffer[command_history_end % MAX_COMMAND_HISTORY];
	
	if (command_history_backtrack >= command_history_end) {
		memset(input_buffer, 0, MAX_LINE_LENGTH);
		command_history_backtrack = command_history_end;
		input_length = 0;
		input_cursor = 0;
		return;
	}
	
	memcpy(input_buffer, history_select, MAX_LINE_LENGTH);
	
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

buffer_t *con_buffer(void) {
	return &console_buffer;
}
