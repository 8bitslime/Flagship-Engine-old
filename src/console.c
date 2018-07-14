#include "console.h"
#include "cmd.h"
#include "text.h"
#include <memory.h>

#define MAX_INPUT_BUFFER 255

static int  input_cursor = 0;
static char input_buffer[MAX_INPUT_BUFFER + 1] = {0};

static void con_exec_input(void) {
	cmd_exec(input_buffer);
	input_cursor = 0;
	memset(input_buffer, 0, MAX_INPUT_BUFFER);
}

void con_draw(int w, int h) {
	text_draw(input_buffer, 0, 0);
}

void con_input(char c) {
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