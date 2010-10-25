#ifndef ASM_PGTABLE_H
#define ASM_PGTABLE_H

#include "asm/page.h"

#define pages_to_mb(x) ((x) >> (20 - PAGE_SHIFT))

#endif
