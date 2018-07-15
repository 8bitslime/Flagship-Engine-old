#pragma once

void con_init(void);
void con_draw(int w, int h);
void con_input(char c);
void con_print(const char *string);
void con_printf(const char *format, ...);

void con_seekUp(void);
void con_seekDown(void);

char *con_buffer(void);