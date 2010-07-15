#ifndef PRINT_H
#define PRINT_H
#include "type.h"

// Clear screen
void cls();

// Convert int to ascii.
void itoa(u_char *buf, int base, int num);

// Display a char.
void putchar(int c);

// A C lib look-like print function.
printf(const char *format, ...);

#endif
