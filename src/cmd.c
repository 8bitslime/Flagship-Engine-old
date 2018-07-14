#include "cmd.h"
#include "cvar.h"
#include <string.h>
#include <stdio.h>

#define MAX_BUCKETS 512

int cmd_argc = 0;
char *cmd_argv[MAX_ARGV] = {NULL};

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

void cmd_exec(const char *command) {
	cmd_tokenize(command);
	
	if (!cmd_argc) {
		return;
	}
	
	cvar_t *cvar;
	cmd_t *cmd;
	
	if ((cvar = cvar_find(cmd_argv[0]))) {
		if (cmd_argc == 1) {
			printf("%s\n", cvar->value);
		} else {
			cvar_setc(cvar, cmd_argv[1]);
		}
	} else if ((cmd = cmd_find(cmd_argv[0]))) {
		cmd->func();
	}
}

void cmd_tokenize(const char *string) {
	for (;cmd_argc;) {
		cmd_argv[--cmd_argc] = NULL;
	}
	
	char buffer[1024] = {0};
	strncpy(buffer, string, 1023);
	char *runner = buffer;
	
	int quotes = 0;
	while (*runner) {
		int i;
		for (i = 0; runner[i] != '\0'; i++) {
			while (*runner == ' ' && !quotes) {
				runner++;
			}
			
			if (runner[i] == '"') {
				if (quotes) {
					runner[i] = '\0';
					quotes = !quotes;
					break;
				} else {
					quotes = !quotes;
					if (i > 0) {
						runner[i] = '\0';
						break;
					}
					runner++; --i;
				}
			} else if (runner[i] == ' ' && !quotes) {
				runner[i] = '\0';
				break;
			}
		}
		if (*runner != '\0') {
			cmd_argv[cmd_argc++] = runner;
		}
		runner = runner + i + 1;
	}
}

static void cmd_echo(void) {
	for (int i = 1; i < cmd_argc; i++) {
		printf("%s ", cmd_argv[i]);
	}
	printf("\n");
}

static cmd_t echo = {"echo", cmd_echo};

void cmd_init(void) {
	cmd_register(&echo);
}