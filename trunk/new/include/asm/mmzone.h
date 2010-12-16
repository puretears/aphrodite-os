#ifndef ASM_MMZONE_H
#define ASM_MMZONE_H

#include "asm/dma.h"
#include "linux/bootmem.h"

#define L1_CACHE_BYTES (1 << 6)
#define SMP_CACHE_BYTES L1_CACHE_BYTES

#define alloc_bootmem(x) \
	__alloc_bootmem_core(NODE_DATA(0), (x), SMP_CACHE_BYTES, __pa(MAX_DMA_ADDRESS))


#endif
