#include "page.h"
#include "mm.h"
#include "head.h"

void paging_init(int start_mem, int end_mem) {
	unsigned int *pg_dir; // Page directory entry
	unsigned int address = 0; // Page table entry
	start_mem = PAGE_ALIGN(start_mem);

	pg_dir = swapper_pg_dir;

	while (address < end_mem) {
		tmp = *(pg_dir + 768);

		if (!tmp) { // A NULL page directory.
			tmp = start_mem | PAGE_TABLE;
			*(pg_dir + 768) = tmp;
			start_mem += PAGE_SIZE;
		}

		*pg_dir++ = tmp; // Also map it at 0x00000000
		pg_table = (u_int *)(tmp & PAGE_MASK);

		for (tmp = 0; tmp < PTE_PER_PAGE; tmp++, pg_table++) {
			if (address < end_mem)
				*pg_table = address | PAGE_SHARED;
			else
				*pg_table = 0;
			pg_table += PAGE_SIZE;
		}
	}
	
	invalidate();
	return start_mem;
} 
