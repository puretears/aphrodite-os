#include "memory.h"
#include "linux/types.h"	// for NULL
#include "linux/head.h"	// for pde definition

#ifdef DEBUG
#include <iostream>
#include <iomanip>
#include <new>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#endif

extern u_char empty_zero_page[1024];
extern int edata, end;

static const u_int PARAM = reinterpret_cast<u_int>(empty_zero_page);
static const u_int TOTAL_MEM = (*(u_int *)(PARAM + 4));


#ifdef DEBUG
void memory::paging_init(u_int *p) {
#else
void memory::paging_init() {
#endif
	start_mem = PAGE_ALIGN(start_mem);
#ifdef DEBUG
	u_int *pg_dir = p;
#else
	u_int *pg_dir = pde;
#endif
	u_int address = 0;
	u_int tmp = 0;

#ifdef DEBUG
	u_int pg_dir_count = 0;
#endif
	u_int *pg_table;

	while (address < end_mem) {
		tmp = *(pg_dir + 768);	

		if (!tmp) { // A new page directory entry
			tmp = start_mem | PAGE_TABLE;
			*(pg_dir + 768) = tmp;
			start_mem += PAGE_SIZE;
		}
		*pg_dir = tmp;	// Also map it at 0x00000000
#ifdef DEBUG
		cout<<"pde["<<pg_dir_count++<<"]"<<*pg_dir<<endl;
#endif
		pg_dir++;
		pg_table = (u_int *)(tmp & PAGE_MASK);

		for (int i = 0; i < 1024; pg_table++, i++) {
			if (address < end_mem)
				*pg_table = address | PAGE_TABLE;
			else
				*pg_table = 0;
#ifdef DEBUG
//			cout<<"pg_table["<<i<<"] "<<(*pg_table)<<endl;
#endif
			address += PAGE_SIZE;
		}
	}
	return;
}

void memory::mem_init(u_int low_start_mem, u_int start_mem, u_int end_mem) {	
}

memory::memory() {
	end_mem = TOTAL_MEM;
	end_mem &= PAGE_MASK;
	
	if (reinterpret_cast<u_int>(&end) >= (1024 * 1024)) {
		start_mem = reinterpret_cast<u_int>(&end);
		low_mem_start = PAGE_SIZE;
	} else {
		start_mem = 1024 * 1024;
		low_mem_start = reinterpret_cast<u_int>(&end);
	}
	low_mem_start = PAGE_ALIGN(low_mem_start);
	paging_init();	
}

#ifdef DEBUG
void no_more_memory() {
	cout<<"no more memory!!!"<<endl;
	exit(-1);
}

bool memory::demo_test() {
	set_new_handler(no_more_memory);

	// Allocate a block of mem buf demo (64MB).
	u_char *mem = new u_char[0x4000000];
	u_char *aligned_mem = NULL;
	memset(mem, 0, 0x4000000);

	if ((reinterpret_cast<u_int>(mem)) & (PAGE_SIZE - 1)) {
		cout<<"Before align, mem = 0x"
			<<hex<<reinterpret_cast<u_long>(mem)
			<<endl;
		aligned_mem = reinterpret_cast<u_char *>
				((reinterpret_cast<u_int>(mem + PAGE_SIZE - 1)) & PAGE_MASK); 
		cout<<"After aligned, mem = 0x"
			<<hex<<reinterpret_cast<u_int>(aligned_mem)
			<<endl;
	}
	u_int *_pde = reinterpret_cast<u_int *>(aligned_mem + 0x1000);
	u_int *pte0 = reinterpret_cast<u_int *>(aligned_mem + 0x2000);

	// PDE and the 1st PTE are initialized by head.S
	_pde[0] = reinterpret_cast<u_int>(&pte0[0]) + 7;
	_pde[768] = reinterpret_cast<u_int>(&pte0[0]) + 7;

	for (int i = 0, j = 0; i < 1024; i++) {
		pte0[i] = j | 7;
		j += 0x1000;
	}

	start_mem = reinterpret_cast<u_int>(aligned_mem + 0x100000);
#ifdef DEBUG
	cout<<"start_mem: 0x"
		<<setw(8)<<setfill('0')<<uppercase<<start_mem
		<<setfill(' ')<<endl;
#endif
	end_mem = 0x2000000;
	paging_init(_pde);
#ifdef DEBUG
	cout<<"start_mem: 0x"
		<<setw(8)<<setfill('0')<<uppercase<<start_mem
		<<setfill(' ')<<endl;
#endif

	delete[] mem;
}
#endif
