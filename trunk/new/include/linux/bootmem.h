#ifndef LINUX_BOOTMEM_H
#define LINUX_BOOTMEM_H

typedef struct bootmem_data {
	/* Physical address of the representing block.*/
	unsigned long node_boot_start;
	/* The PFN of end memory, in the other words, the end of
	 * the ZONE_NORMAL node.*/
	unsigned long node_low_pfn;
	/* The bitmap representing allocated or free pages.*/
	void *node_bootmem_map;
	/* The offset within the page of the end of the last allocation.
	 * If 0, the page is full.*/
	unsigned long last_offset;
	/* The PFN of the page used with the last allocation.*/
	unsigned long last_pos;
	unsigned long last_success;
} bootmem_data_t;

extern void * __init __alloc_bootmem_core(struct bootmem_data *bdata,
		unsigned long size, unsigned long align, unsigned long goal);

#endif
