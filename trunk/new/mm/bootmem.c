#include "asm/page.h"
#include "linux/init.h"
#include "linux/mmzone.h"
#include "linux/bootmem.h"
#include "linux/string.h"
#include "linux/compiler.h"
#include "asm/bitops.h"
#include "asm/bug.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

extern struct pglist_data *pgdat_list;


static void __init reserve_bootmem_core(bootmem_data_t *bdata,
		unsigned long addr, unsigned long size) {
	int i;
	/* 
	 * Round up: partially reserved pages are considered fully reserved.
	 * */
	unsigned sidx = (addr - bdata->node_boot_start) / PAGE_SIZE;
	unsigned eidx = (addr + size - bdata->node_boot_start + 
			PAGE_SIZE - 1) / PAGE_SIZE;

	unsigned int end = (addr + size + PAGE_SIZE - 1) / PAGE_SIZE;

	if ((!size) || (sidx >= eidx) || ((addr >> PAGE_SHIFT) >= bdata->node_low_pfn) ||
			(end > bdata->node_low_pfn)) {
		// BUGs HERE!!!
		return;
	}
	
	for (i = sidx; i < eidx; i++) {
		test_and_set_bit(i, bdata->node_bootmem_map);
	}
}

static void __init free_bootmem_core(bootmem_data_t *bdata, 
		unsigned long addr, unsigned long size) {
	unsigned long i;
	unsigned long start;
	unsigned long sidx;
	/*
	 * Round up: partially free pages are considered reserved.
	 * */
	unsigned long eidx = (addr + size - bdata->node_boot_start) / PAGE_SIZE;
	unsigned long end = (addr + size) / PAGE_SIZE;

	if (end > bdata->node_low_pfn) {
		// BUGs HERE!!!!
		return;
	}

	if (addr < bdata->last_success) {
		bdata->last_success = addr;
	}

	start = (addr + PAGE_SIZE - 1) / PAGE_SIZE;
	sidx = start - (bdata->node_boot_start / PAGE_SIZE);

	for (i = sidx; i < eidx; i++) {
		if(unlikely(!test_and_clear_bit(i, bdata->node_bootmem_map))) {
			// BUGs HERE!!!!!
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

/* Reserve all memories between start and end.*/
static unsigned long __init init_bootmem_core(pg_data_t *pgdat, 
		unsigned long mapstart, unsigned long start, unsigned long end) {
	bootmem_data_t *bdata = pgdat->bdata;
	unsigned long mapsize = ((end - start) + 7) / 8;

	pgdat->pgdat_next = pgdat_list;
	pgdat_list = pgdat;

	mapsize = (mapsize + sizeof(long) - 1) & ~(sizeof(long) - 1);
	bdata->node_bootmem_map = __va(mapstart << PAGE_SHIFT);
	bdata->node_boot_start = (start << PAGE_SHIFT);
	bdata->node_low_pfn = end;

	/* Initially all pages are reserved.*/
	memset(bdata->node_bootmem_map, 0xFF, mapsize);
	return mapsize;
}


// Marks the pages between the address and address+size reserved. Requests
// to partially reserve a pge will result in the full page being reserved.
void reserve_bootmem(unsigned long addr, unsigned long size) {
	reserve_bootmem_core(NODE_DATA(0)->bdata,  addr, size);
}

// Marks the pages between addr and addr+size as free
void free_bootmem(unsigned long addr, unsigned long size) {
	free_bootmem_core(NODE_DATA(0)->bdata, addr, size);
}

// Initialize the memory between 0 and PFN page. The beginning of usable
// memory is at the PFN start
unsigned long __init init_bootmem(unsigned long start, unsigned long page) {
	max_low_pfn = page;
	min_low_pfn = start;
	return (init_bootmem_core(NODE_DATA(0), start, 0, page));
}

/*
 * The core boot memory allcation funcion.
 * @bdata: Point to the current allocation memory node. For UMA, point 
 * to global contig_bootmem_data.
 * @size: The requested allocation size.
 * @align: The allocation base memory alignment.
 * @goal: The preferred allocation base address.
 *
 * Return: The base address of requested buffer, NULL for failed.
 * */
static void * __init __alloc_bootmem_core(struct bootmem_data *bdata, 
		unsigned long size,
		unsigned long align, unsigned long goal) {
	unsigned long eidx, preferred, areasize, incr;
	unsigned long offset = 0;

	if (!size) {
		printk_new("__alloc_bootmem_core(): zero-sized request.\n");
		BUG();
	}

	BUG_ON(align & (align - 1));

	// @eidx: How may pages in the current node.
	eidx = bdata->node_low_pfn - ((bdata->node_boot_start) >> PAGE_SHIFT);

	/* If the alignment of the base address of current node is less than
	 * the alignment of @align, caculate the offset.*/
	if (align && ((bdata->node_boot_start & (align - 1)) != 0)) {
		offset = align - (bdata->node_boot_start & (align - 1));
	}
	offset >>= PAGE_SHIFT;

	/* Try to allocate pages above 'goal' first.*/
	if (goal && 
			(goal >= bdata->node_boot_start) &&
			((goal >> PAGE_SHIFT) < bdata->node_low_pfn)) {
		preferred = goal - bdata->node_boot_start;

		if (bdata->last_success >= preferred)
			preferred = bdata->last_success;
	}
	else {
		preferred = 0;
	}
	/* Make the distance between @goal and base address of current node
	 * aligned to @align.
	 * */
	preferred = ((preferred + align - 1) & ~(align - 1)) >> PAGE_SHIFT;
	preferred += offset;

	areasize = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	incr = align >> PAGE_SHIFT ? 0 : 1;
}
