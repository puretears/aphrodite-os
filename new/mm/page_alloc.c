#include "linux/mmzone.h"
#include "linux/bootmem.h"


struct pglist_data *pgdat_list;

static bootmem_data_t contig_bootmem_data;
struct pglist_data contig_page_data = { .bdata = &contig_bootmem_data };
