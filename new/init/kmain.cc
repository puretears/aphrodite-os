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
	
	//for(; p_mmap < (pmb->mmap_addr + pmb->mmap_length); ) {

		/*printk("0x%x%x limit = %x%x type = %d.\n",
				pmb->mmap_addr->base >> 32, pmb->mmap_addr->base,
				pmb->mmap_addr->limit >> 32, pmb->mmap_addr->limit,
				pmb->mmap_addr->type);	*/

		printk("%x%x\n", pmb->mmap_addr->limit, pmb->mmap_addr->limit);	
		printk("%x\n", pmb->mmap_addr->limit);	
	//	p_mmap = (mmap *)(p_mmap + sizeof(p_mmap->size) + p_mmap->size);
	//}
	return;
}

#ifdef __cplusplus
}
#endif
