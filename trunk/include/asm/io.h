#ifndef IO_H
#define IO_H

#define inb(port)  ({ \
	unsigned char _v; \
	__asm__ volatile ("inb %%dx, %%al":"=a" (_v):"d" (port)); \
	_v; \
})

#define inb_p(port) ({ \
	unsigned char _v; \
	__asm__ volatile ( \
		"inb %%dx, %%al\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:":"=a (_v)":"d" (port)); \
	_v; \
})

#define outb(value, port) ({ \
	__asm__ volatile ("outb %%al, %%dx\n"::"a" (value), "d" (port)); \
})

#define outb_p(value, port) ({ \
	__asm__ volatile ( \
		"outb %%al, %%dx\n" \
		"\tjmp 1f\n" \
		"1:\tjmp 1f\n" \
		"1:"::"a" (value), "d" (port)); \		
})
#endif
