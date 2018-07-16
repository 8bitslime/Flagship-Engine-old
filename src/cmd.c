#include "cmd.h"
#include "flagship.h"
#include "cvar.h"
#include "buffer.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKETS 512

int cmd_argc = 0;
char *cmd_argv[MAX_ARGV] = {NULL};

#define MAX_COMMAND_BUFFER 8192
static buffer_t command_buffer;

static unsigned long hash_str(const char *string) {
	unsigned long out = 5381, c;
	
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	
	return out % MAX_BUCKETS;
}

static cmd_t *cmds = NULL;
static cmd_t *cmds_map[MAX_BUCKETS] = {NULL};

void cmd_register(cmd_t *cmd) {
	unsigned long hash = hash_str(cmd->name);
	
	if (cmds_map[hash] != NULL) {
		cmd->next = cmds_map[hash]->next;
		cmds_map[hash]->next = cmd->next;
	} else {
		cmd->next = cmds;
		cmds = cmd;
		cmds_map[hash] = cmd;
	}
}

cmd_t *cmd_find(const char *name) {
	unsigned long hash = hash_str(name);
	cmd_t *var = cmds_map[hash];
	
	if (var == NULL) {
		return NULL;
	}
	
	while (strcmp(name, var->name) != 0) {
		var = var->next;
		if (var == NULL || (hash_str(var->name) != hash)) {
			return NULL;
		}
	}
	
	return var;
}

void cmd_addString(const char *string) {
	size_t length = strlen(string);
	char *buffer = buffer_getSpace(&command_buffer, length + 1);
	
	if (buffer == NULL) {
		con_print("!command buffer overflow!\n");
		return;
	}
	
	memcpy(buffer, string, length);
	buffer[length] = '\n';
}

static bool_t command_buffer_wait = false;

void cmd_execBuffer(void) {
	char *buffer = (char*)command_buffer.data;
	char line[1024];
	int i = 0, start = 0;
	
	while (i < command_buffer.size) {
		
		if (command_buffer_wait) {
			memcpy(buffer, buffer + i, command_buffer.size - i);
			command_buffer.size -= i;
			command_buffer_wait = false;
			return;
		}
		
		bool_t quotes = false;
		for (; i < command_buffer.size; i++) {
			if (buffer[i] == '"') {
				quotes = !quotes;
			} else if (buffer[i] == ';' && !quotes){
				break;
			} else if (buffer[i] == '\n') {
				break;
			}
		}
		
		memcpy(line, buffer + start, i - start);
		line[i - start] = '\0';
		cmd_exec(line);
		
		start = ++i;
	}
	
	buffer_empty(&command_buffer);
}

void cmd_execFile(const char *fileName) {
	FILE *file = fopen(fileName, "r");
	
	if (file == NULL) {
		con_printf("!could not open file \"%s\"!\n", fileName);
		return;
	}
	
	fseek(file, 0, SEEK_END);
	unsigned long length = ftell(file);
	rewind(file);
	
	char *buffer = malloc(length + 1);
	fread(buffer, sizeof(char), length, file);
	buffer[length] = '\0';
	
	cmd_addString(buffer);
	free(buffer);
}

void cmd_exec(const char *command) {
	char buffer[1024];
	strncpy(buffer, command, 1024);
	
	cmd_tokenize(buffer);
		
	if (!cmd_argc) {
		return;
	}
		
	cmd_t *cmd = cmd_find(cmd_argv[0]);
	
	if (cmd != NULL) {
		cmd->func();
	}
}

void cmd_tokenize(char *buffer) {
	for (;cmd_argc;) {
		cmd_argv[--cmd_argc] = NULL;
	}
	
	// char buffer[1024] = {0};
	// strncpy(buffer, string, 1023);
	char *runner = buffer;
	
	int quotes = 0;
	while (*runner) {
		int i;
		for (i = 0; runner[i]; i++) {
			while (*runner == ' ' && !quotes) {
				runner++;
			}
			
			if (runner[i] == '"') {
				if (quotes) {
					quotes = !quotes;
					break;
				} else {
					quotes = !quotes;
					if (i) {
						break;
					}
					runner++; --i;
				}
			} else if (runner[i] == ' ' && !quotes) {
				break;
			}
		}
		if (*runner) {
			cmd_argv[cmd_argc++] = runner;
		}
		
		if (runner[i]) {
			runner[i] = '\0';
			runner += i + 1;
		} else {
			break;
		}
	}
}

static void cmd_quit_f(void) {
	exit(0);
}

static void cmd_echo_f(void) {
	for (int i = 1; i < cmd_argc; i++) {
		con_printf("%s ", cmd_argv[i]);
	}
	con_print("\n");
}

static void cmd_wait_f(void) {
	command_buffer_wait = true;
}

static void cmd_exec_f(void) {
	if (cmd_argc < 2) {
		con_print("?exec <file name>\n");
	} else {
		cmd_execFile(cmd_argv[1]);
	}
}

static cmd_t quit = {"quit", cmd_quit_f};
static cmd_t echo = {"echo", cmd_echo_f};
static cmd_t wait = {"wait", cmd_wait_f};
static cmd_t exec = {"exec", cmd_exec_f};

void cmd_init(void) {
	cmd_register(&quit);
	cmd_register(&echo);
	cmd_register(&wait);
	cmd_register(&exec);
	
	buffer_alloc(&command_buffer, MAX_COMMAND_BUFFER);
}
