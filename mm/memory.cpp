#include "memory.h"
#include "linux/types.h"	// for NULL
#include "linux/head.h"	// for pde definition
#ifdef DEBUG
#include <iostream>
#include <new>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#endif
#ifdef DEBUG
unsigned int memory::paging_init(unsigned int *p) {
#else
unsigned int memory::paging_init() {
#endif
	start_mem = PAGE_ALIGN(start_mem);
	unsigned int *pg_dir = p;
	unsigned int address = 0;
	unsigned int tmp = 0;

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

		for (int i = 0; i < 1024; pg_table++, i++) {
			if (address < end_mem)
				*pg_table = address | PAGE_TABLE;
			else
				*pg_table = 0;
			address += PAGE_SIZE;
		}
	}
	return start_mem;
}

memory::memory(size_t start, size_t end)
	:start_mem(start), end_mem(end) {
	//paging_init();	
}
#ifdef DEBUG
void no_more_memory() {
	cout<<"no more memory!!!"<<endl;
	exit(-1);
}

bool memory::demo_test() {
	set_new_handler(no_more_memory);

	// Allocate a block of mem buf demo (64MB).
	unsigned char *mem = new unsigned char[0x4000000];
	unsigned char *aligned_mem = NULL;
	memset(mem, 0, 0x4000000);

	if ((reinterpret_cast<unsigned int>(mem)) & (PAGE_SIZE - 1)) {
		cout<<"Before align, mem = 0x"
			<<hex<<reinterpret_cast<unsigned long>(mem)
			<<endl;
		aligned_mem = reinterpret_cast<unsigned char *>
				((reinterpret_cast<unsigned int>(mem + PAGE_SIZE - 1)) & PAGE_MASK); 
		cout<<"After aligned, mem = 0x"
			<<hex<<reinterpret_cast<unsigned long>(aligned_mem)
			<<endl;
	}
	unsigned int *_pde = reinterpret_cast<unsigned int *>(aligned_mem + 0x1000);
	unsigned int *pte0 = reinterpret_cast<unsigned int *>(aligned_mem + 0x2000);

	// PDE and the 1st PTE are initialized by head.S
	_pde[0] = reinterpret_cast<unsigned int>(&pte0[0]) + 7;
	_pde[768] = reinterpret_cast<unsigned int>(&pte0[0]) + 7;

	for (int i = 0, j = 0; i < 1024; i++) {
		pte0[i] = j | 7;
		j += 0x1000;
	}

	

	start_mem = reinterpret_cast<unsigned int>(aligned_mem + 0x100000);
	end_mem = 0x2000000;
	paging_init(_pde);


	delete[] mem;
}
#endif
