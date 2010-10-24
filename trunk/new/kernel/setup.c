#include "asm/e820.h"

struct e820map e820;

unsigned long init_pg_tables_end __initdata = ~0UL;

static unsigned long __init setup_memory() {
	min_low_pfn = PFN_UP(init_pg_tables_end);

	find_max_pfn();
}

void __init find_max_pfn() {
	int i = 0;

	max_pfn = 0;

	for (; i < e820.nr_map; i++) {
		unsigned long start, end;

		if (e820.map[i].type != E820_ram)
			continue;

		start = PFN_UP(e820.map[i].addr);
		end = PFN_DOWN(e820.map[i].addr + e820.map[i].size);

		if (start >= end)
			continue;

		if (end > max_pfn)
			max_pfn = end;
	}
}
