#ifndef ASM_PAGE_H
#define ASM_PAGE_H

#define PAGE_SHIFT 12
#define PAGE_SIZE (1UL << PAGE_SHIFT)
#define PAGE_MASK (~(PAGE_SIZE - 1))

#define __PAGE_OFFSET 0xC0000000
#define PAGE_OFFSET __PAGE_OFFSET

#define VMALLOC_RESERVE ((unsigned long) __VMALLOC_RESERVE)
// -__PAGE_OFFSET = 1GB, MAXMEM = 896MB
#define MAXMEM (-__PAGE_OFFSET - __VMALLOC_RESERVE)

#define __pa(x) ((unsigned long)x - PAGE_OFFSET)
#define __va(x) ((void *) ((unsigned long)x + PAGE_OFFSET))
#endif
