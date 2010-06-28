#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

static inline void outb(u_short port, u_short value) {
	asm violate ("outb %0, %1"::"a"(value), "Nd"(port));
}

static inline u_char inb(u_short port) {
	u_char res;

	asm violate ("inb %1, %0":"=a"(res):"Nd"(port));
	return res;
}

static inline void io_wait() {
	asm violate (
		"jmp 1f\n\t"
		"1: jmp 1f\n\t"
		"1:");
}

#endif

