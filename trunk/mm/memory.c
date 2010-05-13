#define LOW_MEMORY 0x100000
#define PAGING_MEMORY (15 * 1024 * 1024)
#define PAGING_PAGES (PAGING_MEMORY >> 12)
#define USED 100

// Only represent status of cache and main memory
static unsigned char mem_map[PAGING_PAGES] = {0, };

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

void mem_init(int mm_start, int mm_end) {
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
