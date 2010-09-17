extern do_divide_error
extern do_debug
extern do_nmi
extern do_ini3
extern do_overflow
extern do_bound_range_exceeded
extern do_invalid_opcode
extern do_device_not_available
extern do_double_fault
extern do_coprocessor_segment_overrun
extern do_stack_segment_fault
extern do_general_protection
extern do_page_fault
extern do_x87_error
extern do_alignment_check
extern do_machine_check
extern do_smid_exception
extern do_reserved 

global ret_from_system_call
global divide_error
global debug
global nmi
global int3
global overflow
global bound_rangd_exceeded
global invalied_opcode
global device_not_available
global double_fault
global coprocessor_segment_overrun
global invalid_tss
global segment_not_present
global stack_segment_fault
global general_protection
global page_fault
global x87_error
global alignment_check
global machine_check
global smid_exception
global reserved
global system_call

section .text

KERNEL_CS equ	010H
KERNEL_DS equ 018H

USER_CS equ 023H
USER_DS	equ 02BH

_EBX	equ	0
_ECX	equ 4
_EDX	equ 8
_ESI	equ 0CH
_EDI	equ 10H
_EBP	equ 14H
_EAX	equ 18H
_DS		equ 1CH
_ES		equ	20H
_FS		equ 24H
_GS		equ	28H
_0_EAX	equ	2CH
_EIP	equ	30H
_CS		equ	34H
_EFLAGS	equ	38H
_OLD_ESP	equ 3CH
_OLD_SS		equ	40H

%macro RESTORE_ALL 0
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop eax
	pop ds
	pop es
	pop fs
	pop gs
	add esp, 4	
	iret
%endmacro

%macro SAVE_ALL 0
	push es
	push ds
	push eax
	push ebp
	push edi
	push esi
	push edx
	push ecs
	push ebx
%endmacro

re_from_system_call:
	RESTORE_ALL

align 4

divide_error:
	push	0	;only for some exp
	SAVE_ALL
	push do_divide_error
	nop
align 4

error_code:
	xchg eax, [esp + _0_EAX + 4]	;error code	
	push eax, 
	lea edx, [esp + 8]	;preg_t address  EIP
	push edx
	mov dx, KERNEL_DS	;	0x18H???
	mov ds, dx
	mov es, dx
	call [esp + 8]		;	EIP
