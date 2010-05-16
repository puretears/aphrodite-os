#define LOW_MEMORY 0x100000
#define PAGING_MEMORY (15 * 1024 * 1024)
#define PAGING_PAGES (PAGING_MEMORY >> 12)
#define USED 100

// Only represent status of cache and main memory
static unsigned char mem_map[PAGING_PAGES] = {0, };
static unsigned int HIGH_MEMORY = 0;

inline unsigned int map_nr(unsigned int addr) {
	return ((addr - LOW_MEMORY) >> 12);
}

unsigned int get_free_page() {
	register unsigned int __res asm("eax");

	__asm__(
		"std\n\t"
		"repne scasb\n\t"
		"jne 1f\n\t"
		"sall $12, %%ecx\n\t"
		"addl %2, %%ecx\n\t"
		"mov %%ecx, %%edx\n\t"
		"mov $1024, %%ecx\n\t"
		"leal 4092(%%edx), %%edi\n\t"
		"rep stosd\n\t"
		"movl %%edx, %%eax\n\t"
		"1:"
		:"=a"(_res)
		:""(0), "i"(LOW_MEMORY), "c"(PAGING_PAGES), 
		 "D"(mem_map + PAGING_PAGES - 1)
		:"edx");
	return __res;
}

void free_page(unsigned int paddr) {
	if ((paddr < LOW_MEMORY) || (paddr > HIGH_MEMORY))
		return; // Kernel and noexisting memory cannot be freed.
	int index = map_nr(paddr);

	if (mem_map[index]--)
		return;

	// !!!Execute here, there must be errors occured.
	mem_map[index] = 0;
	return;

}

inline void invalidate() {
	__asm__("movl %%eax, %%cr3"::"a" (0));
}

void free_page_tables(unsigned int laddr, int size) {
	unsigned int *pg_dir, *pg_table, *pg_dir;
	int nr;

	if (laddr & 0x3FFFFF)
		return; // Beginning address must be aligned with 4MB
	if (!laddr)
		return;

	pg_dir = (unsigned int *)((laddr >> 22) << 2);
	size = (size + 0x3FFFFF) >> 22;

	
	for (; size--; pg_dir++) {
		if (!((*pg_dir) & 1))
			continue; // No existing page directory entry
		pg_table = (unsigned int *)((*pg_dir) & 0xFFFFF000);

		for (nr = 0; nr < 1024; nr++) {
			if ((pg_table[nr]) & 1)
				free_page((pg_table[nr]) & 0xFFFFF000);
			pg_table[nr] = 0;
		}
		free_page((*pg_dir) & 0xFFFFF000)	
		*pg_dir = 0;
	}

	invalidate();
	return 0;

}

void mem_init(int mm_start, int mm_end) {
	HIGH_MEMROY = mem_end;
	int i;

	for (i = 0; i < PAGING_PAGES; i++) {
		mem_map[i] = USED:
	}
	
	i = map_nr(start);
	int j = ((mm_end - mm_start) >> 12);
	while (j--) {
		mem_map[i++] = 0;
	};

	return;
}
