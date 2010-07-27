#ifndef PAGE_H
#define PAGE_H

#define PAGE_SHIFT  12
#define PAGE_SIZE (1 << PAGE_SHIFT)

#define PAGE_ALIGN(n) (n & ~(PAGE_SHIFT))
#endif
