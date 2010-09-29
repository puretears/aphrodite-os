#ifndef PAGE_H
#define PAGE_H

#define PAGE_SHIFT  12
#define PAGE_SIZE (1 << PAGE_SHIFT)

#define PAGE_MASK (~(PAGE_SIZE - 1))
#define PAGE_ALIGN(n) ((n + PAGE_SIZE -1) & PAGE_MASK)

#define PTES_PER_PAGE (PAGE_SIZE / sizeof(int *))
#endif
