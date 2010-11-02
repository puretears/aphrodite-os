#include "asm/page.h"
#include "asm/bootmem.h"
#include "linux/string.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

// Initialize the memory between 0 and PFN page. The beginning of usable
// memory is at the PFN start
unsigned long __init init_bootmem(unsigned long start, unsigned long page) {
	max_low_pfn = pages;
	min_low_pfn = start;
	return (init_bootmem_core(NODE_DATA(0), start, 0, pages));
}

// Marks the pages between the address and address+size reserved. Requests
// to partially reserve a pge will result in the full page being reserved.
void reserve_bootmem(unsigned long addr, unsigned long size) {

}

// Marks the pages between addr and addr+size as free
void free_bootmem(unsigned long addr, unsigned long size) {
	free_bootmem_core(NODE_DATA(0)->bdata, addr, size);
}

static void __init free_bootmem_core(bootmem_data_t *bdata, 
		unsigned long addr, unsigned long size) {
	unsigned long i;
	unsigned long start;
	unsigned long sidx;
	unsigned long eidx = (addr + size - bdata->node_boot_start) / PAGE_SIZE;
	unsigned long end = (addr + size) / PAGE_SIZE;

	if (end > bdata->node_low_pfn) {
		return;
	}

	if (addr < bdata->last_success) {
		bdata->last_success = addr;
	}

	start = (addr + PAGE_SIZE - 1) / PAGE_SIZE;
	sidx = start - (bdata->node_boot_start / PAGE_SIZE);

	for (i = sidx; i < eidx; i++) {
		if(unlikely(!test_and_clear_bit(i, bdata->node_bootmem_map))) {
			return;
		}
	}
}

// Allocates size number of bytes from ZONE_NORMAL. The allocator will be
// aligned to the L1 hardware cache to get the maximum benefit from the
// hardware cache.
void *alloc_bootmem(unsigned long size) {
	
}

// Allocates size number of bytes from ZONE_DMA.
void *alloc_bootmem_low(unsigned long size) {

}

// Allocates size number of bytes from ZONE_NORMAL aligned on a page size
// so the full pages will be returned to the called.
void *alloc_bootmem_pages(unsigned long size) {

}

// Allocates size number of bytes from ZONE_DMA aligned on a page size.
void *alloc_bootmem_low_pages(unsigned long size) {

}

// Calculates the number of pages required to store a bitmap representing
// the allocation of *pages* number of pages.
unsigned long bootmem_bootmap_pages(unsigned long pages) {

}

// Cycles all pages in the bitmap and free the free pages to the physical
// page allocator.
unsigned long free_all_bootmem() {

}


static unsigned long __init init_bootmem_core(pg_data_t *pgdat, 
		unsigned long mapstart, unsigned long start, unsigned long end) {
	bootmem_data_t *bdata = pgdat->bdata;
	unsigned long mapsize = ((end - start) + 7) / 8;

	pgdat->pgdat_next = pg_list;
	pg_list = pgdat;

	mapsize = (mapsize + sizeof(long) - 1) & ~(sizeof(long) - 1);
	bdata->node_bootmem_map = __va(mapstart << PAGE_SHIFT);
	bdata->node_boot_start = (start << PAGE_SHIFT);
	bdata->node_low_pfn = end;

	/* Initially all pages are reserved.*/
	memset(bdata->node_bootmem_map, 0xFF, mapsize);
	return mapsize;
}
