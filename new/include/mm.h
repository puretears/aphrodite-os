#ifndef MM_H
#define MM_H
#include "type.h"

#define PAGE_P 0x001
#define PAGE_RW 0x002
#define PAGE_US 0x004

#define PAGE_ACCESSED 0x020

#define PAGE_TABLE (PAGE_ACCESSED | PAGE_US | PAGE_RW | PAGE_P)
#define PAGE_SHARED (PAGE_ACCESSED | PAGE_US | PAGE_RW | PAGE_P)

#endif
