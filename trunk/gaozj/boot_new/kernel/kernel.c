#include "multiboot.h"
#include "print.h"

int kmain (unsigned int * pmb, unsigned int magic){
	if (magic != 0x2BADB002){
		printk_new("Magic error!");
	}

	multi_info_t  *multiinfo;
	cls();

	multiinfo=(multi_info_t *)pmb;
	mmap *p_mmap;
	p_mmap=(mmap *)multiinfo->mmap_addr;

	printk_new("meminfo \n");
	printk_new("magic number is %8x\n", magic);
	printk_new("mmap_len is %x\n",multiinfo->mmap_length);
	printk_new("mmap_addr is %x\n",multiinfo->mmap_addr);

	for(p_mmap=(mmap *)multiinfo->mmap_addr;

			p_mmap < (mmap *)((u_32)multiinfo->mmap_addr + multiinfo->mmap_length);
			p_mmap = (mmap *)((u_32)p_mmap + p_mmap->size + sizeof(p_mmap->size))){

	//for(;(u_32)p_mmap<(multiinfo->mmap_addr + multiinfo->mmap_length/sizeof(mmap));){
		printk_new("%8x ""%8x ""%8x ""%8x " "%d\n",
					(int)(p_mmap->base),
					(int)(p_mmap->base>>32),
					(int)(p_mmap->limit),
					(int)(p_mmap->limit>>32),
					(int)(p_mmap->type));						
	}
	//printk_new("Total memory size: %dM\n",memory_end/1024/1024);
//	unsigned int * p = pmb;
//	p =0 ;
//	display((const char *)&a);
	return 0;
}
