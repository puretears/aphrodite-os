#ifndef LINUX_MMZONE_H
#define LINUX_MMZONE_H

#include "linux/bootmem.h"

extern struct pglist_data contig_page_data;
#define NODE_DATA(nid) (&contig_page_data)

struct bootmem_data;

typedef struct pglist_data {
	struct bootmem_data *bdata;
	unsigned long node_start_pfn;
	struct pglist_data *pgdat_next;
} pg_data_t;

#endif
