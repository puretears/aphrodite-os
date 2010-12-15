#ifdef __cplusplus
extern "C" {
#endif

#include "linux/mbinfo.h"
#include "linux/print.h"
#include "linux/type.h"
#include "asm/page.h"
#include "asm/e820.h"

#define MAGIC_NUM 0x2BADB002

extern struct e820map e820;

extern char __end;
void trap_init();
int paging_init(int, int);
void init_IRQ(void);
void page_address_init();
void setup_arch();


void print_memory_map(struct mbinfo *pmb) {
	mmap *p_mmap = pmb->mmap_addr;
	
	printk_new("System Address Map:\n");
	printk_new("BaseL    BaseH    LimitL   LimitH   Type\n");

	int memory_end = 0;
	int i = 0;

	for(; p_mmap < (pmb->mmap_addr + pmb->mmap_length / sizeof(mmap)); i++) {
		e820.map[i].addr = p_mmap->addr;
		e820.map[i].size = p_mmap->limit;
		e820.map[i].type = p_mmap->type;

		printk_new("%8x %8x %8x %8x %8d\n",
				(int)(p_mmap->addr), 
				(int)(p_mmap->addr >> 32),
				(int)(p_mmap->limit), 
				(int)(p_mmap->limit >> 32),
				(int)(p_mmap->type));
		if (p_mmap->type == 1) {
			memory_end = (int)(p_mmap->addr + p_mmap->limit);
		}
		p_mmap = (mmap *)((u_int)p_mmap + sizeof(p_mmap->size) + p_mmap->size);
	}

	e820.nr_map = i;
	printk_new("Total memory size: %d MB.\n", (memory_end + 1024 * 1024 - 1)/1024/1024);

	int memory_start = (int)&__end;
	//int low_memory_start = PAGE_SIZE;
	//memory_start = paging_init(memory_start, memory_end);
	printk_new("memory start at 0x%8x.\n", memory_start);
}

#ifdef DEBUG
extern unsigned long swapper_pg_dir;
extern unsigned long pg0;
extern unsigned long init_pg_tables_end;

void bootmem_dbg() {
	int i;
	int ipde = 0;
	unsigned long *pde = &swapper_pg_dir;
	unsigned long *pte = &pg0;

	printk_new("Swapper_pg_dir = %8x.\n", pde);
	while (*(pde) && *(pde +768)) {
		pde++;
		ipde++;
	}
	printk_new("%d page directories entries.\n", ipde * 2);

	int phy_page0_addr = swapper_pg_dir & 0xFFFFF000;
	int j = ((unsigned int)pte - 0xC0000000);

	if (phy_page0_addr != j) {
		// BUGs Here!!!!
		return;
	}

	for (i = 0; i <= ipde; i++) {
		printk_new("pte [%d]: %8x.\n", i, *(&swapper_pg_dir + i));
		printk_new("pte [%d]: %8x.\n", (i + 768), *(&swapper_pg_dir + 768 + i));
		printk_new("pte [%d]: %8x.\n", (i + 768), *((&swapper_pg_dir) + i + 768));
	}

	printk_new("Init_pg_tables_end: %8x.\n", init_pg_tables_end);

	return;
}
#endif

void start_kernel(u_int magic_num, struct mbinfo *pmb) {
	if (magic_num != MAGIC_NUM) {
		printk_new("Invalid kernel image.\n");
	}
	cls();
	print_memory_map(pmb);
#ifdef DEBUG
	bootmem_dbg();
#endif
	page_address_init();
	setup_arch();

	trap_init();
	init_IRQ();

	return;
}

#ifdef __cplusplus
}
#endif
