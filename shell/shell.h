#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
char keyboard_getchar_with_ctrl(void);
int keyboard_has_input(void);
int keyboard_ctrl_pressed(void);
void keyboard_readline(char *buffer, int max_len);

#endif
