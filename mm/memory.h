#ifndef MM_H
#define MM_H
#include "linux/types.h"

#ifdef DEBUG
void no_more_memory();
#endif

class memory {
private:
	// PDE and PTE flags
	static const unsigned int PAGE_P    = 0x001;
	static const unsigned int PAGE_RW   = 0x002;
	static const unsigned int PAGE_USER = 0x004;
	static const unsigned int PAGE_A = 0x020;
	// PTE flags
	static const unsigned int PAGE_D = 0x040;
	// Some general CONSTANTS
	static const unsigned int PAGE_SHIFT = 12;
	static const unsigned int PAGE_SIZE  = ((unsigned int)1 << PAGE_SHIFT);
	static const unsigned int PAGE_MASK  = ~(PAGE_SIZE - 1);

	static const unsigned int PAGE_TABLE  = PAGE_P | PAGE_RW | PAGE_USER | PAGE_A;
	static const unsigned int PAGE_SHARED = PAGE_P | PAGE_RW | PAGE_USER | PAGE_A;
private:
	// Align to the next page boundary
	inline unsigned int PAGE_ALIGN(unsigned int addr) {
		return (addr + PAGE_SIZE - 1) & PAGE_MASK;
	}
	inline void invalidate() {
		__asm__ (
			"movl %%cr3, %%eax\n\t"
			"movl %%eax, %%cr3\n\t"
			:::"eax");
	}
#ifdef DEBUG
	unsigned int paging_init(unsigned int *p);
#else
	unsigned int paging_init();
#endif

private:
	unsigned int start_mem; // Beginning address for locating page tables
	unsigned int end_mem;	// Total memory

public:
	memory(size_t start = 0, size_t end = 0);
#ifdef DEBUG
	bool demo_test();
#endif
};

#endif
