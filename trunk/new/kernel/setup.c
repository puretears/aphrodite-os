#include "asm/e820.h"
#include "asm/setup.h"
#include "linux/init.h"
#include "asm/pgtable.h"

struct e820map e820;

unsigned long init_pg_tables_end __initdata = ~0UL;
static unsigned int highmem_pages = -1;

extern unsigned long max_low_pfn;
extern unsigned long min_low_pfn;
extern unsigned long max_pfn;
extern unsigned int __VMALLOC_RESERVE;

void __init find_max_pfn();
unsigned long __init find_max_low_pfn();
	
static unsigned long __init setup_memory() {
	min_low_pfn = PFN_UP(init_pg_tables_end);

	find_max_pfn();
	max_low_pfn = find_max_low_pfn();

#ifdef CONFIG_HIGHMEM
	highstart_pfn = highend_pfn = max_pfn;

	if (max_pfn > max_low_pfn) {
		highstart_pfn = max_low_pfn;
	}

	printk_new("%dMB HIGHMEM available.\n", 
			pages_to_mb(highend_pfn - highstart_pfn));
#endif

	printk_new("%dMB LOWMEM available.\n",
			pages_to_mb(max_low_pfn));

	setup_bootmem_allocator();

	return max_low_pfn;
}

void __init find_max_pfn() {
	int i = 0;
	max_pfn = 0;
	unsigned long start, end;

	for (; i < e820.nr_map; i++) {

		if (e820.map[i].type != E820_RAM)
			continue;

		start = PFN_UP(e820.map[i].addr);
		end = PFN_DOWN(e820.map[i].addr + e820.map[i].size);

		if (start >= end)
			continue;

		if (end > max_pfn)
			max_pfn = end;
	}
}

unsigned long __init find_max_low_pfn() {
	unsigned long max_low_pfn = max_pfn;

	// If the physical mem is greater than 896MB,
	// max_low_pfn is equal to MAXMEM_PFN.
	if (max_low_pfn > MAXMEM_PFN) {
		if (highmem_pages == -1) {
			highmem_pages = max_pfn - MAXMEM_PFN;
		}
		max_low_pfn = MAXMEM_PFN;
#ifndef CONFIG_HIGHMEM
		printk_new("Warning only %dMB will be used.\n", MAXMEM >> 20);
		if (max_pfn > MAX_NONPAE_PFN)
			printk_new("Use a PAE enabled kernel.\n");
		else
			printk_new("Use a HIGHMEM enabled kernel.\n");
		max_pfn = MAXMEM_PFN;
#else
#ifndef CONFIG_X86_PAE
		if (max_pfn > MAX_NONPAE_PFN) {
			max_pfn = MAX_NONPAE_PFN;
			printk_new("Warning only 4GB will be used.\n");
			printk_new("Use a PAE enabled kernel.\n");
		}
#endif /* !CONFIG_X86_PAE*/
#endif /* !CONFIG_HIGHMEM*/
	}
	else {
		if (highmem_pages == -1)
			highmem_pages = 0;
	}
	return max_low_pfn;
}

static void __init register_bootmem_low_pages(unsigned long max_low_pfn) {
	int i = 0;
	unsigned long curr_pfn, last_pfn, size;
	for (; i < e820.nr_map; i++) {
		if (e820.map[i].type != E820_RAM)
			continue;

		curr_pfn = PAGE_UP(e820.map[i].addr);

		if (curr_pfn >= max_low_pfn)
			continue;

		last_pfn = PAGE_DOWN(e820.map[i].addr + e820.map[i].size);

		if (last_pfn >= max_low_pfn)
			last_pfn = max_low_pfn;

		if (last_pfn <= curr_pfn)
			continue;

		size = last_pfn - curr_pfn;
		free_bootmem(PFN_PHYS(curr_pfn), PFN_PHYS(size));
	}
}

