/*.section ".multiboot_text"*/
.text

#include "linux/linkage.h"
#include "asm/page.h"

/* 128KB for bootmem allocator bitmap.*/
#define INIT_MAP_BEYOND_END (1024 * 768)
#define MAGIC 0x1BADB002
#define MODULEALIGN (1 << 0)
#define MEMINFO (1 << 1)
#define FLAGS (MODULEALIGN | MEMINFO)
#define CHECKSUM (-(MAGIC + FLAGS))

.align 4
MultiBootHeader:
	.long MAGIC
	.long FLAGS
	.long CHECKSUM
/*
	.long MultiBootHeader

	.long _text - __PAGE_OFFSET
	.long _end - __PAGE_OFFSET
	.long _end - __PAGE_OFFSET
	.long multiboot_startup_32 - __PAGE_OFFSET
*/
ENTRY(multiboot_startup_32)
	/* jmp startup_32 - __PAGE_OFFSET*/
	jmp startup_32
