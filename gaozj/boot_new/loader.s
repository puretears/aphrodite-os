global entry_addr
extern load_addr, load_end_addr, bss_end_addr, kmain

MAGIC	equ 1BADB002H
FLAG_ALIGNED		equ	 1<<0
FLAG_MEMINFO	equ	1<<1
FLAG	equ	(FLAG_ALIGNED | FLAG_MEMINFO)
CHECKSUM	equ	-(MAGIC + FLAG)
section .text
align 4
multiboot:
	dd	MAGIC
	dd	FLAG
	dd	CHECKSUM
	dd	multiboot
	dd	load_addr
	dd	load_end_addr
	dd	bss_end_addr
	dd	entry_addr

entry_addr:
	mov esp, stack
	push eax
	push ebx
	call kmain
	hlt
section .bss
align	4
stack:
	resb 0x4000
