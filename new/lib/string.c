#include "string.h"
#include "type.h"

//#include <stdio.h>

int strlen(const char *str) {
	int ilen = 0;
	__asm__ (
		"xorb %%al, %%al\n\t"
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
