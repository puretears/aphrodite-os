#ifndef HEAD_H
#define HEAD_H

#include "linux/type.h"
#include "asm/processor.h"

extern u_int swapper_pg_dir[1024];
extern struct desc_struct idt[256];
extern struct desc_struct gdt[256];

#endif
