.text

#include "linux/linkage.h"

#include "asm/segment.h"
#include "asm/page.h"
#include "asm/thread_info.h"

/* 128KB for bootmem allocator bitmap.*/
#define INIT_MAP_BEYOND_END (1024 * 768)

page_pde_offset = (__PAGE_OFFSET >> 20);

ENTRY(startup_32)
	cld
	lgdt boot_gdt_desc - __PAGE_OFFSET
	movl $(__BOOT_DS), %eax
	movl %eax, %ds
	movl %eax, %es
	movl %eax, %fs
	movl %eax, %gs

	/* Clear bss so that there are no surprises.*/
	xorl %eax, %eax
	movl $__bss_start - __PAGE_OFFSET, %edi
	movl $__bss_stop - __PAGE_OFFSET, %ecx
	subl %edi, %ecx
	shrl $2, %ecx
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
	addl $0x1000, %eax
	loop 11b

	leal (INIT_MAP_BEYOND_END + 0x007)(%edi), %ecx
	cmpl %ecx, %eax
	jb 10b 
	movl %edi, (init_pg_tables_end - __PAGE_OFFSET)

	/* Enable paging*/
	movl $swapper_pg_dir - __PAGE_OFFSET, %eax
	movl %eax, %cr3
	movl %cr0, %eax
	orl $0x80000000, %eax
	movl %eax, %cr0
	ljmp $__BOOT_CS, $1f
1:
	lss stack_start, %esp

	/* Initialize EFLAGS*/
	pushl $0
	popfl

	call setup_idt
	lgdt cpu_gdt_desc
	lidt idt_desc
	ljmp $(__KERNEL_CS), $1f
1:
	movl $(__KERNEL_DS), %eax
	movw %ax, %ss

	movl $(__USER_DS), %eax
	movw %ax, %ds
	movw %ax, %es

	xorl %eax, %eax /* Clear FS/GS and LDT*/
	movw %ax, %fs
	movw %ax, %es
	lldt %ax

	cld
	call start_kernel
L6:
	jmp L6 # Main should never return here.
	
setup_idt:
	lea ignore_int, %edx
	movl $(__KERNEL_CS << 16), %eax
	movw %dx, %ax
	movw 0x8E00, %dx

	lea idt_table, %edi
	movl $256, %ecx

rp_sidt:
	movl %eax, (%edi)
	movl %edx, 4(%edi)
	addl $8, %edi
	decl %ecx
	jne rp_sidt
	ret


	/* The default interrupt handler.*/
ignore_int:
	cld
#ifdef CONFIG_PRINTK
	pushal
	pushl %es
	pushl %ds
	movl  $(__KERNEL_DS), %eax
	movw  %ax, %es
	movw  %ax, %ds
	movl  40(%esp), %eax
	pushl %eax
	pushl $int_msg
	call  printk_new
	addl  $8, %esp
	popl %ds
	popl %es
	popal
#endif
	iret

.section ".bss.page_aligned", "w"
ENTRY(swapper_pg_dir)
	.fill 1024, 4, 0
ENTRY(empty_zero_page)
	.fill 4096, 1, 0
	
.data
ENTRY(stack_start)
	.long init_thread_union + THREAD_SIZE
	.long __BOOT_DS

int_msg:
	.asciz "Unknown interrupt or fault at EIP %8X.\n"

.global boot_gdt_desc
.global idt_desc
.global cpu_gdt_desc
	
/* All pesudo descriptors definitions.*/
ALIGN
	.word 0
boot_gdt_desc:
	.word __BOOT_DS + 7
	.long boot_gdt_table - __PAGE_OFFSET

	.word  0
idt_desc:
	.word IDT_ENTRIES * 8 - 1
	.long idt_table

	.word 0
cpu_gdt_desc:
	.word GDT_ENTRIES * 8 - 1
	.long cpu_gdt_table

/* GDT definitions*/
ENTRY(boot_gdt_table)
	.fill GDT_ENTRY_BOOT_CS, 8, 0
	.quad 0x00CF9A000000FFFF
	.quad 0x00CF92000000FFFF


ENTRY(cpu_gdt_table)
	.quad 0x0000000000000000        /* NULL descriptor */
	.quad 0x0000000000000000        /* 0x0b reserved */
	.quad 0x0000000000000000        /* 0x13 reserved */
	.quad 0x0000000000000000        /* 0x1b reserved */
	.quad 0x0000000000000000        /* 0x20 unused */
	.quad 0x0000000000000000        /* 0x28 unused */
	.quad 0x0000000000000000        /* 0x33 TLS entry 1 */
	.quad 0x0000000000000000        /* 0x3b TLS entry 2 */
	.quad 0x0000000000000000        /* 0x43 TLS entry 3 */
	.quad 0x0000000000000000        /* 0x4b reserved */
	.quad 0x0000000000000000        /* 0x53 reserved */
	.quad 0x0000000000000000        /* 0x5b reserved */

	.quad 0x00cf9a000000ffff        /* 0x60 kernel 4GB code at 0x00000000 */
	.quad 0x00cf92000000ffff        /* 0x68 kernel 4GB data at 0x00000000 */
	.quad 0x00cffa000000ffff        /* 0x73 user 4GB code at 0x00000000 */
	.quad 0x00cff2000000ffff        /* 0x7b user 4GB data at 0x00000000 */

	.quad 0x0000000000000000        /* 0x80 TSS descriptor */
	.quad 0x0000000000000000        /* 0x88 LDT descriptor */

	/* Segments used for calling PnP BIOS */
	.quad 0x00c09a0000000000        /* 0x90 32-bit code */
	.quad 0x00809a0000000000        /* 0x98 16-bit code */
	.quad 0x0080920000000000        /* 0xa0 16-bit data */
	.quad 0x0080920000000000        /* 0xa8 16-bit data */
	.quad 0x0080920000000000        /* 0xb0 16-bit data */
	/*
	 * The APM segments have byte granularity and their bases
	 * and limits are set at run time.
	 */
	.quad 0x00409a0000000000        /* 0xb8 APM CS    code */
	.quad 0x00009a0000000000        /* 0xc0 APM CS 16 code (16 bit) */
	.quad 0x0040920000000000        /* 0xc8 APM DS    data */

	.quad 0x0000920000000000        /* 0xd0 - ESPFIX 16-bit SS */
	.quad 0x0000000000000000        /* 0xd8 - unused */
	.quad 0x0000000000000000        /* 0xe0 - unused */
	.quad 0x0000000000000000        /* 0xe8 - unused */
	.quad 0x0000000000000000        /* 0xf0 - unused */
	.quad 0x0000000000000000        /* 0xf8 - GDT entry 31: double-fault TSS */
