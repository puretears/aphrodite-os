#ifdef __cplusplus
extern "C" {
#endif

#include "linux/mbinfo.h"
#include "linux/print.h"
#include "linux/type.h"
#include "asm/page.h"

#define MAGIC_NUM 0x2BADB002

extern char end;
void trap_init();
int paging_init(int, int);
void init_IRQ(void);

void kmain(mbinfo *pmb, u_int magic_num) {
	if (magic_num != MAGIC_NUM) {
		printk_new("Invalid kernel image.\n");
	}

	cls();

	mmap *p_mmap = pmb->mmap_addr;
	
	printk_new("System Address Map:\n");
	printk_new("BaseL    BaseH    LimitL   LimitH   Type\n");

	int memory_end = 0;
	int i = 0;
	for(; p_mmap < (pmb->mmap_addr + pmb->mmap_length / sizeof(mmap)); i++) {
		e820.map[i].addr = p_mmap->mmap_addr;
		e820.map[i].size = p_mmap->limit;
		e820.map[i].type = p_mmap->type;

		printk_new("%8x %8x %8x %8x %8d\n",
				(int)(p_mmap->base), 
				(int)(p_mmap->base >> 32),
				(int)(p_mmap->limit), 
				(int)(p_mmap->limit >> 32),
				(int)(p_mmap->type));
		if (p_mmap->type == 1) {
			memory_end = (int)(p_mmap->base + p_mmap->limit);
		}
		p_mmap = (mmap *)((u_int)p_mmap + sizeof(p_mmap->size) + p_mmap->size);
	}
	e820.nr_map = i;

	printk_new("Total memory size: %d.\n", memory_end/1024/1024);
	int memory_start = (int)&end;
	int low_memory_start = PAGE_SIZE;
	memory_start = paging_init(memory_start, memory_end);
	printk_new("memory start at 0x%8x.\n", memory_start);
	trap_init();
	init_IRQ();

	return;
}

#ifdef __cplusplus
}
#endif
