#ifndef MM_H
#define MM_H
#include "linux/types.h"

#ifdef DEBUG
void no_more_memory();
#endif

class memory {
private:
	// PDE and PTE flags
	static const u_int PAGE_P    = 0x001;
	static const u_int PAGE_RW   = 0x002;
	static const u_int PAGE_USER = 0x004;
	static const u_int PAGE_A = 0x020;
	// PTE flags
	static const u_int PAGE_D = 0x040;
	// Some general CONSTANTS
	static const u_int PAGE_SHIFT = 12;
	static const u_int PAGE_SIZE  = ((u_int)1 << PAGE_SHIFT);
	static const u_int PAGE_MASK  = ~(PAGE_SIZE - 1);

	static const u_int PAGE_TABLE  = PAGE_P | PAGE_RW | PAGE_USER | PAGE_A;
	static const u_int PAGE_SHARED = PAGE_P | PAGE_RW | PAGE_USER | PAGE_A;

private:
	inline u_int MAP_NR(u_int addr) { return addr >> PAGE_SHIFT; }
	// Align to the next page boundary
	inline u_int PAGE_ALIGN(u_int addr) {
		return (addr + PAGE_SIZE - 1) & PAGE_MASK;
	}
	inline void invalidate() {
		__asm__ (
			"movl %%cr3, %%eax\n\t"
			"movl %%eax, %%cr3\n\t"
			:::"eax");
	}

#ifdef DEBUG
	void paging_init(u_int *p);
#else
	void paging_init();
#endif
	void mem_init(u_int low_start_mem,  u_int start_mem, u_int end_mem);
private:
	u_int low_mem_start;
	u_int start_mem;	// Beginning address for locating page tables
	u_int end_mem;		// Total memory
	u_int high_mem;		// Physical address of the last page frame
	u_char *mem_map;

public:
	memory();
#ifdef DEBUG
	bool demo_test();
#endif
};

#endif
