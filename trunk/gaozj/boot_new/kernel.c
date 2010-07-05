#include<multiboot.h>
int kmain (void * pmb, unsigned int magic){
	if (magic != 0x1BADB002){
		magic = 0x1badb002;
	}
	unsigned int * p = pmb;
	p =0 ;
	return 0;
}
