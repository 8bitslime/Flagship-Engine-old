#pragma once

typedef enum {
	CVAR_FLOATING = 0,
	CVAR_INTEGER
} cvar_flags_t;

typedef struct cvar_t {
	const char *name;
	char *value;
	cvar_flags_t flags;
	union {
		int   ival;
		float fval;
	};
	struct cvar_t *next;
} cvar_t;

void cvar_register(cvar_t *cvar);
cvar_t *cvar_find(const char *name);

void cvar_set(const char *name, const char *string);
void cvar_setc(cvar_t *cvar, const char *string);
void cvar_setf(const char *name, float f);
void cvar_seti(const char *name, int i);