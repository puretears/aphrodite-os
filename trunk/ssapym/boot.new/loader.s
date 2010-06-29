global loader
extern code,end,bss,kmain

MAGIC_NUMBER	equ 0x1BADB002
FLAGS_ALIGNED	equ 1<<0
FLAGS_MEMINFO	equ 1<<1
FLAGS		equ (FLAGS_ALIGNED | FLAGS_MEMINFO)
CHECKSUM	equ -(MAGIC_NUMBER + FLAGS)

section .text
align 4
multiboot:
	dd MAGIC_NUMBER
	dd FLAGS
	dd CHECKSUM
	dd multiboot
	dd code
	dd end
	dd bss
	dd loader

loader:
	mov esp, stack
	push eax
	push ebx
	call kmain
	hlt

section .bss
align 4
stack:
	resb 0x4000
