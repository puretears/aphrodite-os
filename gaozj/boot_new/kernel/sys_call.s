extern do_divide_error
extern do_debug
extern do_nmi
extern do_int3
extern do_overflow
extern do_bound_range_exceeded
extern do_invalid_opcode
extern do_device_not_available
extern do_double_fault
extern do_coprocessor_segment_overrun
extern do_invalid_tss
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
_0_EAX	equ	2CH			; orig_eax 中断号
_EIP	equ	30H			; cs , EIP 返回地址 
_CS		equ	34H		
_EFLAGS	equ	38H
_OLD_ESP	equ 3CH		;用户堆栈空间
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
	add esp, 4	
	iret
%endmacro

%macro SAVE_ALL 0	;保存现场
	push es
	push ds
	push eax
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
%endmacro

ret_from_exception:
	RESTORE_ALL

align 4

divide_error:
	push 0 ;only for some exp
	push do_divide_error	
	jmp error_code

error_code:
	
	push ds
	push eax
	xor eax, eax			;eax =1 
	push ebp
	push edi 
	push esi
	push edx 
	dec eax			;eax =1
	push ecx
	push ebx 
	cld		; clear DF

	mov ecx, es
	mov esi, [esp + _0_EAX]		;GET error code
	mov edi, [_ES + esp]	;get the fuction address
	mov eax, [esp + _0_EAX]	;	set _0_EAX = -1 no syscall to restart
	mov [_ES + esp], ecx;		
	mov edx, esp
	push esi			;error code
	push edx			;  pr_regs pointer
	mov edx, KERNEL_DS
	mov ds, edx
	mov es, edx
	call edi
	add esp, 8
	jmp ret_from_exception

align 4
debug:
	push 0
	push do_debug
	jmp error_code

align 4
	push 0
	push do_nmi
	jmp error_code

align 4
int3:
	push 0
	push do_int3
	jmp error_code

align 4
overflow:
	push 0
	push do_overflow
	jmp error_code

align 4
bounds:
	push 0
	push do_bound_range_exceeded
	jmp error_code

align 4
invalid_op:
	push 0
	push do_invalid_opcode
	jmp error_code

align 4
device_not_available:
	push 0
	push do_device_not_available
	jmp error_code

align 4
coprocessor_segment_overrun:
	push 0
	push do_coprocessor_segment_overrun
	jmp error_code

align 4
invalid_tss:
	push 0
	push do_invalid_tss
	jmp error_code

align 4
segment_not_present:
	push 0
	push do_segment_not_present
	jmp error_code

align 4
stack_segment:
	push 0
	push do_stack_segment
	jmp error_code

align 4
general_protection:
	push 0
	push do_general_protection
	jmp error_code

align 4
page_fault:
	push 0
	push do_page_fault
	jmp error_code

align 4
x87_error:
	push 0
	push do_x87_error
	jmp error_code

align 4
alignment_check:
	push 0
	push do_alignment_check
	jmp error_code

align 4
machine_check:
	push 0
	push do_machine_check
	jmp error_code

align 4
smid_exception:
	push 0
	push do_smid_exception
	jmp error_code

align 4
system_call:
	push 0
	push do_system_call
	jmp error_code
