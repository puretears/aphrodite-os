

_idt:
	times 512 dd 0

align 2
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

align 2
	dw 0
_gdt_pesudo:
	dw 256 * 8 - 1
	dd _gdt
