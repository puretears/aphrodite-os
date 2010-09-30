#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "type.h"

/*TODO:
 * You should add outw and outl functions respectively.
 * The same as inb, inw and inl.
 * */
static inline void outb(u_short port, u_char value) {
	asm volatile ("outb %0, %1"::"a"(value), "Nd"(port));
}

static inline u_char inb(u_short port) {
	u_char res;

	asm volatile ("inb %1, %0":"=a"(res):"Nd"(port));
	return res;
}

static inline void io_wait() {
	asm volatile (
		"jmp 1f\n\t"
		"1: jmp 1f\n\t"
		"1:");
}
void io_delay(int s){
	int i;
	for(i=0; i<s ; i++)
	{
		io_wait();
	}
}

#endif

