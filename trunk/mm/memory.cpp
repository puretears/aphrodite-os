#include "memory.h"

extern unsigned int* pde;
#define NULL 0

unsigned int memory::paging_init() {
	start_mem = PAGE_ALIGN(start_mem);
	unsigned int *pg_dir = pde;
	unsigned int address = 0;
	unsigned int tmp = NULL;

	while (address < end_mem) {
		tmp = *(pg_dir + 768);	

		if (!tmp) { // A new page directory entry
			tmp = start_mem | PAGE_TABLE;
			*(pg_dir + 768) = tmp;
			start_mem += PAGE_SIZE;
		}
		*pg_dir = tmp;	// Also map it at 0x00000000
		pg_dir++;

		unsigned int *pg_table = (unsigned int *)(tmp & PAGE_MASK);

		for (int i = 0; i < 1024; pg_table++) {
			if (address < end_mem)
				*pg_table = address | PAGE_TABLE;
			else
				*pg_table = 0;
			address += PAGE_SIZE;
			
		}
	}
	return start_mem;
}



