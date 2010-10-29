#ifndef LINUX_BOOTMEM_H
#define LINUX_BOOTMEM_H

typedef struct bootmem_data {
	unsigned long node_boot_start;
	usnigned long node_low_pfn;
	void *node_bootmem_map;
	unsigned long last_offset;
	unsigned long last_pos;
	unsigned long last_success;
} bootmem_data_t;

#endif
