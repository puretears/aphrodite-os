%include "../boot/protect.inc"
extern user_stack

[SECTION .text]
BITS 32
pde:	; Page directory will be stored here
global startup32

startup32:

	mov ax, 010H
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, user_stack

times (01000H - ($ - $$)) db 0
pte0:

times (02000H - ($ - $$)) db 0
pte1:

times (03000H - ($ - $$)) db 0
pte2:

times (04000H - ($ - $$)) db 0
pte3:

times (05000H - ($ - $$)) db 0


; 256 Interrupt descriptors
_idt:
	times 512 dd 0
align 4
	dw 0
_idt_pesudo:
	dw 256 * 8 - 1
	dd _idt

; 256 GDT entries
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
