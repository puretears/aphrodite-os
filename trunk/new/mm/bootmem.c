
// Initialize the memory between 0 and PFN page. The beginning of usable
// memory is at the PFN start
unsigned long init_bootmem(unsigned long start, unsigned long page) {

}

// Marks the pages between the address and address+size reserved. Requests
// to partially reserve a pge will result in the full page being reserved.
void reserve_bootmem(unsigned long addr, unsigned long size) {

}

// Marks the pages between addr and addr+size as free
void free_bootmem(unsigned long addr, unsigned long size) {

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
