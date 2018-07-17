#include "cvar.h"
#include "cmd.h"
#include "console.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUCKETS 512

static unsigned long hash_str(const char *string) {
	unsigned long out = 5381, c;
	
	while ((c = *string++)) {
		out = ((out << 5) + out) + c;
	}
	
	return out % MAX_BUCKETS;
}

static cvar_t *cvars = NULL;
static cvar_t *cvars_map[MAX_BUCKETS] = {NULL};

static void cmd_list_f(void) {
	cvar_t *var = cvars;
	
	for (; var != NULL; var = var->next) {
		con_printf("?%s: %s\n", var->name, var->value);
	}
}

static void cmd_set_f(void) {
	if (cmd_argc == 1) {
		con_printf("?set <cvar> <value>\n");
	} else {
		cvar_t *var = cvar_find(cmd_argv[1]);
		
		if (var == NULL) {
			con_printf("!could not find \"%s\"!\n", cmd_argv[1]);
			return;
		}
		
		if (cmd_argc == 2) {
			con_printf("?%s: %s\n", var->name, var->value);
		} else {
			cvar_set(cmd_argv[1], cmd_argv[2]);
		}
	}
}

static cmd_t cmd_list = {"cvars", cmd_list_f};
static cmd_t cmd_set  = {"set", cmd_set_f};

void cvar_init(void) {
	cmd_register(&cmd_list);
	cmd_register(&cmd_set);
}

void cvar_register(cvar_t *cvar) {
	unsigned long hash = hash_str(cvar->name);
	
	int length = strlen(cvar->value);
	//TODO: custom allocator
	char *value = malloc(length + 1);
	strncpy(value, cvar->value, length);
	value[length] = '\0';
	
	cvar->value = value;
	
	if (cvar->flags & CVAR_INTEGER) {
		cvar->ival = strtol(cvar->value, NULL, 10);
	} else {
		cvar->fval = strtof(cvar->value, NULL);
	}
	
	if (cvars_map[hash] != NULL) {
		
		con_printf("!hash colission\n");
		
		cvar->next = cvars_map[hash]->next;
		cvars_map[hash]->next = cvar;
	} else {
		cvar->next = cvars;
		cvars = cvar;
		cvars_map[hash] = cvar;
	}
}

cvar_t *cvar_find(const char *name) {
	unsigned long hash = hash_str(name);
	cvar_t *var = cvars_map[hash];
	
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

void cvar_set(const char *name, const char *string) {
	cvar_t *var = cvar_find(name);
	if (var != NULL) {
		cvar_setc(var, string);
	} 
}

void cvar_setc(cvar_t *cvar, const char *string) {
	free(cvar->value);
	
	int length = strlen(string);
	//TODO: custom allocator
	char *value = malloc(length + 1);
	strncpy(value, string, length);
	value[length] = '\0';
	
	cvar->value = value;
	
	if (cvar->flags & CVAR_INTEGER) {
		cvar->ival = strtol(cvar->value, NULL, 10);
	} else {
		cvar->fval = strtof(cvar->value, NULL);
	}
}

void cvar_setf(const char *name, float f) {
	cvar_t *var = cvar_find(name);
	
	if (var == NULL) {
		return;
	}
	
	var->fval = f;
	
	free(var->value);
	char *buffer = malloc(32);
	snprintf(buffer, 32, "%f", f);
	var->value = buffer;
}

void cvar_seti(const char *name, int i) {
	cvar_t *var = cvar_find(name);
	
	if (var == NULL) {
		return;
	}
	
	var->ival = i;
	
	free(var->value);
	char *buffer = malloc(32);
	snprintf(buffer, 32, "%i", i);
	var->value = buffer;
}
