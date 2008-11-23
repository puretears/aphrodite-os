%include "sconst.inc" 

__NR_get_ticks equ 0
INT_VECTOR_SYS_CALL equ 144

; real system calls are defined here
global get_ticks

[section .text]
bits 32
get_ticks:
	mov eax, __NR_get_ticks
	int INT_VECTOR_SYS_CALL
	ret
