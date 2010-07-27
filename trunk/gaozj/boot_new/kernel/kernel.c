#include "multiboot.h"
#include "../include/print.h"
extern void display(const char *);

int kmain (unsigned int * pmb, unsigned int magic){
	if (magic != 0x1BADB002){
		magic = 0x1badb002;
	}
	cls();
	printk_new("magic number is %8x\n", magic);
	
//	unsigned int * p = pmb;
//	p =0 ;
//	display((const char *)&a);
	return 0;
}
