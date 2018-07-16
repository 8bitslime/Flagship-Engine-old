#pragma once

void con_init(void);
void con_draw(int w, int h);

void con_input(char c);
void con_historyUp(void);
void con_historyDown(void);
void con_cursorLeft(void);
void con_cursorRight(void);
void con_cursorHome(void);
void con_cursorEnd(void);

void con_print(const char *string);
void con_printf(const char *format, ...);

void con_seekUp(void);
void con_seekDown(void);

char *con_buffer(void);
