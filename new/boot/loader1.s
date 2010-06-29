global loader
extern text, end, bss, loader

MODULEALIGN equ	1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ	1<<1                   ; provide memory map
FLAGS       equ MODULEALIGN | MEMINFO  ; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002           ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required

section .text
align 4
MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd MultiBootHeader
	; The follow variables are defined in linker.ld
	dd text
	dd end
	dd bss
	dd loader
STACKSIZE equ 0x4000

loader:
	; Possibly set up a stack here: mov esp, stack + STACKSIZE
	push eax                        ; Multiboot magic number
	push ebx                        ; Multiboot info structure
		  
	call kmain
	hlt
section .bss
align 4
stack:
	resb STACKSIZE
