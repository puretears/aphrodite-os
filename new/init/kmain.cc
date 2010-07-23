#ifdef __cplusplus
extern "C" {
#endif

#include "mbinfo.h"
#include "print.h"
#include "type.h"
#define MAGIC_NUM 0x2BADB002


void kmain(mbinfo *pmb, u_int magic_num) {
	if (magic_num != MAGIC_NUM) {
		printk("Invalid kernel image.\n");
	}

	cls();

	mmap *p_mmap = pmb->mmap_addr;
	
	for(; p_mmap < (pmb->mmap_addr + pmb->mmap_length / sizeof(mmap)); ) {

		printk("base = 0x%x%x limit = %x%x type = %d.\n",
				(int)(p_mmap->base >> 32), 
				(int)(p_mmap->base),
				(int)(p_mmap->limit >> 32), 
				(int)(p_mmap->limit),
				(int)(p_mmap->type));

		p_mmap = (mmap *)((char *)p_mmap + sizeof(p_mmap->size) + p_mmap->size);
	}
	return;
}

#ifdef __cplusplus
}
#endif
