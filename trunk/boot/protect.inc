
%macro descriptor 3
	dw (%2 & 0FFFFH)
	dw (%1 & 0FFFFH)
	db ((%1 >> 16) & 0FFH)
	dw ((%3 & 0F0FFH) | ((%2 >> 4) & 0F00H))
	db ((%1 >> 24) & 0FFH)
%endmacro


PTE_P equ 0001H
PTE_US equ 0002H
PTE_RW equ 0004H

PDE_P equ 0001H
PDE_US equ 0002H
PDE_RW equ 0004H
