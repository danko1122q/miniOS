#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
char keyboard_getchar(void);
int keyboard_has_input(void);
void keyboard_readline(char *buffer, int max_len);

#endif
