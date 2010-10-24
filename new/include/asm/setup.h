#ifndef ASM_SETUP_H
#define ASM_SETUP_H

#include "asm/page.h"

#define PFN_UP(x) (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x) ((x) >> PAGE_SHIFT)

#endif
