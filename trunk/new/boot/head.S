.text

#include <linux/linkage.h>

#include <asm/segment.h>
#include <asm/page.h>
#include <asm/thread_info.h>

/* 128KB for bootmem allocator bitmap.*/
#define INIT_MAP_BEYOND_END (1024 * 768)

page_pde_offset = (__PAGE_OFFSET >> 20);

ENTRY(startup_32)
	cld
	lgdt boot_gdt_desc - __PAGE_OFFSET
	movl $(__BOOT_DS), %eax
	movl %ax, %ds
	movl %ax, %es
	movl %ax, %fs
	movl %ax, %gs

	/* Clear bss so that there are no surprises.*/
	xorl %eax, %eax
	movl __bss_start - __PAGE_OFFSET, %edi
	movl __bss_stop - __PAGE_OFFSET, %ecx
	subl %edi, %ecx
	shrl %ecx, 2
	rep ; stosl

	/* Init page tables.*/
	movl $(pg0 - __PAGE_OFFSET),  %edi
	movl $(swapper_pg_dir - __PAGE_OFFSET), %edx
	movl $0x007, %eax /* Present + RW + User*/
10:
	leal 0x007(%edi), %ecx /* Create a PDE entry*/
	movl %ecx, (%edx)		/* Store identity PDE entry*/
	movl %ecx, page_pde_offset(%edx) /* Store kernel PDE entry*/
	addl $4, %edx
	movl $1024, %ecx

11: /* Init pg0*/
	stosl
	addl$0x1000, %eax
	loop 11b

	leal (INIT_MAP_BEYOND_END + 0x007)(%edi), %ecx
	cmpl %ecx, %eax
	jb 10b 
	movl %edi, (init_pg_tables_end - __PAGE_OFFSET)

	/* Enable paging*/
	movl $swapper_pg_dir - __PAGE_OFFSET, %eax
	movl %eax, %cr3
	movl %cr0, %eax
	orl 0x80000000, %eax
	movl %eax, %cr0
	ljmp %__BOOT_CS, $1f
1:
	lss stack_start, %esp

.section ".bss.page_aligned", "w"
ENTRY(swapper_pg_dir)
	.fill 1024, 4, 0
ENTRY(empty_zero_page)
	.fill 4096, 1, 0
	
.data
ENTRY(stack_start)
	.long init_thread_union + THREAD_SIZE
	.long __BOOT_DS

ALIGN
	.word 0
boot_gdt_desc:
	.word __BOOT_DS + 7
	.long boot_gdt_table - __PAGE_OFFSET

ENTRY(boot_gdt_table)
	.fill GDT_ENTRY_BOOT_CS, 8, 0
	.quad 0x00CF9A000000FFFF
	.quad 0x00CF92000000FFFF
