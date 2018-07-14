#pragma once

typedef void (*cmdfun_t)(void);
typedef struct cmd_t {
	char *name;
	cmdfun_t func;
	struct cmd_t *next;
} cmd_t;

#define MAX_ARGV 128

extern int cmd_argc;
extern char *cmd_argv[MAX_ARGV];

void cmd_register(cmd_t *cmd);
cmd_t *cmd_find(const char *name);

void cmd_exec(const char *command);
void cmd_tokenize(char *string);

void cmd_init(void);