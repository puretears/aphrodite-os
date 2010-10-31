#include "linux/string.h"
#include "linux/type.h"

//#include <stdio.h>

inline void *memset(void *s, char c, size_t count) {
	int d0, d1;
	__asm__ __volatile__(
			"rep \n\t"
			"stosb"
			:"=&c" (d0), "=&D" (d1) 
			:"a" (c), "1"(s), "0"(count) 
			: "memory");
	return s;
}

int strlen(const char *str) {
	int ilen = 0;
	__asm__ (
		"xorb %%al, %%al\n\t"
		"cld\n\t"
		"1:\n\t"
		"scasb\n\t"
		"jz 1f\n\t"
		"incl %1\n\t"
		"jmp 1b\n\t"
		"1:"
		::"D" (str), "m" (ilen)
	);
	return ilen;
}


/*int main(int argc, char **argv) {
	int i = strlen1(argv[1]);
	printf("Length is: %d.\n", i);
	return 0;
}*/
