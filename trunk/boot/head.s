SECTION .text
pde:	; Page directory will be stored here
_startup32:

	mov ax, 010H
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, as

org 01000H
pte0:

org 02000H
pte1:

org 03000H
pte2:

org 04000H
pte3:

org 05000H


_idt:
	times 512 dd 0

align 4
	dw 0
_idt_pesudo:
	dw 256 * 8 - 1
	dd _idt

%include "../boot/protect.inc"
_gdt:
_dummy: gdt_desc 0, 0, 0
_code:  gdt_desc 0FFFH, 0, 0C9AH
_data:  gdt_desc 0FFFH, 0, 0C92H
_tmp:   gdt_desc 0, 0, 0
	times 504 dd 0

align 4
	dw 0
_gdt_pesudo:
	dw 256 * 8 - 1
	dd _gdt
