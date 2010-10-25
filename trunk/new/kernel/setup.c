#include "asm/e820.h"

struct e820map e820;

unsigned long init_pg_tables_end __initdata = ~0UL;
static unsigned int highmem_pages = -1;

static unsigned long __init setup_memory() {
	min_low_pfn = PFN_UP(init_pg_tables_end);

	find_max_pfn();
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
	}
	else {
		if (highmem_pages == -1)
			highmem_pages = 0;
	}
	return max_low_pfn;
}
