; Limit, Base address, Attribute
%macro gdt_desc 3
	dw %1 & 0FFFFH
	dw %2 & 0FFFFH
	db (%2 >> 16) & 0FFH
	dw ((%1 >> 4) & 0F00H) | (%3 & 0F0FFH)
	db (%2 >> 24) & 0FFH
%endmacro

; Offset, Selector, Attribute
%macro idt_desc 3
	dw %1 & 0FFFFH
	dw %2 & 0FFFFH
	dw (%3 & 0EE00H) | 0600H
	dw (%1 >> 16) & 0FFFFH
%endmacro
