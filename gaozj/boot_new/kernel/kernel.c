#include "multiboot.h"
#include "print.h"
#include "mm.h"
int page_init(int,int);
void trap_init();
void init_IRQ();
extern char load_end_addr;
int kmain (unsigned int * pmb, unsigned int magic){
	if (magic != 0x2BADB002){
		printk_new("Magic error!");
	}

	multi_info_t  *multiinfo;
	cls();
	
	multiinfo=(multi_info_t *)pmb;
	mmap *p_mmap;
	p_mmap=(mmap *)multiinfo->mmap_addr;
	int mem_end = 0;

	printk_new("meminfo \n");
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
		if(p_mmap->type == 1){
			mem_end = (int)(p_mmap->base + p_mmap->limit);
		}
		
	}
	printk_new("Total memory size: %dM\n",mem_end/1024/1024);
	int mem_start=(int)&load_end_addr;
	mem_start = page_init(mem_start,mem_end);
	printk_new("mem_start is %8x",mem_start);
	trap_init();
	return 0;
}
