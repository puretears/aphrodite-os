#include "type.h"
#include "mm.h"
#include "page.h"
int page_init(int start_mem, int end_mem){
	u_int address = 0 ;	
	u_int *pg_dir;
	u_int *pg_table;

	start_mem = PAGE_ALIGN(start_mem);
	pg_dir = swapper_pg_dir;

	while (address < end_mem){
		tmp = *(pg_dir +768);
		if(!tmp){
			tmp = start_mem | PAGE_TABLE;
			*(pg_dir + 768) = tmp;
			start_mem += PAGE_SIZE;		// + 4k 
		}
		*pg_dir++ = tmp;
		pg_table = (u_int *)(tmp & PAGE_MASK);
		for(tmp =0; tmp < PRES_PER_PAGE; tmp++, pg_table++){
			if(address <end_mem)
			  *page_talbe = address | PAGE_SHARED;
			else 
				*pg_table = 0;
			address += PAGE_SIZE;
		}  
	}
	invalidate();
	return start_mem;
}
