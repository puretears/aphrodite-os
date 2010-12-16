#include "asm/page.h"
#include "linux/page.h"
#include "linux/mm.h"
#include "linux/head.h"

int paging_init(int start_mem, int end_mem) {
	unsigned int *pg_dir; // Page directory entry
	unsigned int *pg_table;
	unsigned int pg_count = 0;
	unsigned int pte_count = 0;
	unsigned int address = 0; // Page table entry
	start_mem = PAGE_ALIGN(start_mem);
	u_int tmp = 0;
	pg_dir = swapper_pg_dir;

	while (address < end_mem) {
		tmp = *(pg_dir + 768);
	//	printk_new("pg%1d: %8x.\n", pg_count++, pg_dir);
		if (!tmp) { // A NULL page directory.
			tmp = start_mem | PAGE_TABLE;
			*(pg_dir + 768) = tmp;
			start_mem += PAGE_SIZE;
		}

		*pg_dir++ = tmp; // Also map it at 0x00000000
		pg_table = (u_int *)(tmp & PAGE_MASK);
	//	printk_new("pt%1d: %8x.\n", pte_count++, pg_table);

		for (tmp = 0; tmp < PTES_PER_PAGE; tmp++, pg_table++) {
			if (address < end_mem)
				*pg_table = address | PAGE_SHARED;
			else
				*pg_table = 0;
			address += PAGE_SIZE;
		}
	}
	
	invalidate();
	return start_mem;
} 
