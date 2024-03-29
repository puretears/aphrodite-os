#include "asm/page.h"
#include "asm/mmzone.h"
#include "linux/init.h"
#include "linux/bitops.h"
#include "linux/kernel.h"
#include "linux/mmzone.h"
#include "linux/bootmem.h"
#include "linux/string.h"
#include "linux/page.h"
#include "linux/compiler.h"
#include "asm/bitops.h"
#include "asm/bug.h"

unsigned long max_low_pfn;
unsigned long min_low_pfn;
unsigned long max_pfn;

extern struct pglist_data *pgdat_list;
extern bootmem_data_t contig_bootmem_data;

void * __init __alloc_bootmem_core(struct pglist_data *bdata,
		unsigned long size, unsigned long align, unsigned long goal); 

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
	__alloc_bootmem_core(NODE_DATA(0), size, SMP_CACHE_BYTES, 0);	
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
void * __init __alloc_bootmem_core(struct pglist_data *pdata, 
		unsigned long size,
		unsigned long align, unsigned long goal) {
	unsigned long eidx, preferred, areasize, incr, remaining_size;
	unsigned long offset = 0;
	struct bootmem_data *bdata = pdata->bdata;
	void *ret;

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
	/* Both @offset and @preferred are aligned to @align.
	 * Total pages we need to move from preferred starting address to
	 * node_boot_start.*/
	preferred += offset;

	areasize = (size + PAGE_SIZE - 1) / PAGE_SIZE;
	incr = align >> PAGE_SHIFT ? : 1;

	int i = 0, j = 0, start = 0;
restart_scan:
	for (i = preferred; i < eidx; i += incr) {
		i = find_next_zero_bit(bdata->node_bootmem_map, eidx, i);
		i = ALIGN(i, incr);
		/* The page after align must be still available, else continue to 
		 * scan.*/	
		if (test_bit(i, bdata->node_bootmem_map))
			continue;

		// Here we find a nice beginning page.
		// Continue to scan whether we can get enough page from this beginning
		// page.	
		for (j = i; j < (i + areasize); j++) {
			if ((j >= eidx) || test_bit(j, bdata->node_bootmem_map))
				// Cannot get enough pages.
				goto fail_block;
		}
		// We've got it.
		start = i;
		goto found;
		
fail_block:
		i = ALIGN(j, incr);
	} // End for (i = preferred ...)

	// If i >=  eidx, we'll get here. It says that we cannot get enough pages
	// from the preferred address. Try to get pages from 0
	if (preferred > offset) {
		preferred = offset;
		goto restart_scan;
	}
	// We cannot get pages neither from preferred address nor 0.
	return NULL;

found:
	bdata->last_success = start << PAGE_SHIFT;

	/* Deciding if this allocation can be merged with previous one.*/
	if (align < PAGE_SIZE  &&
		bdata->last_offset &&
		(bdata->last_pos + 1) == start) {
		/* The beginning address must be aligned with @align.*/
		offset = (bdata->last_offset + align - 1) & ~(align - 1);	
		BUG_ON(offset > PAGE_SIZE);
		/* How many bytes in the previous allocated page?*/
		remaining_size = PAGE_SIZE - offset;

		if (size < remaining_size) {
			areasize = 0;
			/* last_pos unchanged.*/
			bdata->last_offset = offset + size;
			ret = __va(bdata->last_pos * PAGE_SIZE + offset + bdata->node_boot_start);
		}
		else {
			remaining_size = size - remaining_size;
			areasize = (remaining_size + PAGE_SIZE - 1) / PAGE_SIZE;
			ret = __va(bdata->last_pos * PAGE_SIZE + offset + bdata->node_boot_start);

			bdata->last_pos = start + areasize - 1;
			bdata->last_offset = remaining_size;
		} // End if (size < remaining_size)

		bdata->last_offset &= ~PAGE_MASK;
	}
	else {
		bdata->last_pos = start + areasize - 1;
		bdata->last_offset = size & ~PAGE_MASK;
		ret = __va(start * PAGE_SIZE + bdata->node_boot_start);
	} // End if (align < PAGE_SIZE ...)

	/* Reserve the pages now.*/
	for (i = start; i < (start + areasize); i++) {
		if (unlikely(test_and_set_bit(i, bdata->node_bootmem_map)))
			BUG();
	}

	memset(ret, 0, size);
	return ret;

}
