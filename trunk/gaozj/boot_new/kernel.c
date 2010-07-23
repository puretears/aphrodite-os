#include "multiboot.h"
extern void display(const char *);

int kmain (unsigned int * pmb, unsigned int magic){
	if (magic != 0x1BADB002){
		magic = 0x1badb002;
	}
	
//	unsigned int * p = pmb;
//	p =0 ;
	int a=0x0a;
	display((const char *)&a);
	return 0;
}
