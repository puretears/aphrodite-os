#ifndef IO_H
#define IO_H

static inline unsigned char inb(unsigned short port) {
	unsigned char _v;
	__asm__ volatile ("inb %%dx, %%al":"=a" (_v):"d"(port));
	return _v;
}

static inline unsigned char inb_p(unsigned short port) {
	unsigned char _v;
	__asm__ volatile ("inb %%dx, %%al\n"
			"\tjmp 1f\n"
			"1:\tjmp 1f\n"
			"1:":"=a" (_v):"d"(port));
	return _v;
}

static inline void outb(unsigned char val, unsigned short port) {
	__asm__ volatile ("outb %%al, %%dx"::"a" (val), "d" (port));
}

static inline void outb_p(unsigned char val, unsigned short port) {
	__asm__ volatile ("outb %%al, %%dx\n"
			"\tjmp 1f\n"
			"1:\tjmp 1f\n"
			"1:"::"a" (val), "d" (port));
}

#endif
