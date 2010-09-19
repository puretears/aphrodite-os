extern do_divide_error
extern do_debug
extern do_nmi
extern do_int3
extern do_overflow
extern do_bounds
extern do_invalid_op
extern do_device_not_available
extern do_double_fault
extern do_coprocessor_segment_overrun
extern do_invalid_tss
extern do_segment_not_present
extern do_stack_segment
extern do_general_protection
extern do_page_fault
extern do_x87_error
extern do_alignment_check
extern do_machine_check
extern do_smid_exception
extern do_reserved
extern do_system_call

global ret_from_system_call
global divide_error
global debug
global nmi
global _int3
global overflow
global bounds
global invalid_op
global device_not_available
global double_fault
global coprocessor_segment_overrun
global invalid_tss
global segment_not_present
global stack_segment
global general_protection
global page_fault
global x87_error
global alignment_check
global machine_check
global smid_exception
global reserved
global system_call

section .text
; Stack layout in 'ret_from_system_call'
; +38 - old_ss
; +34 - old_esp
; +30 - eflags
; +2C - cs
; +28 - eip
; +24 - orig_eax (err_code)
; +20 - es
; +1C - ds
; +18 - eax
; +14 - ebp
; +10 - edi
; +0C - esi
; +08 - edx
; +04 - ecx
; +00 - ebx  <== esp

KERNEL_CS equ 010H
KERNEL_DS equ 018H

USER_CS equ 023H
USER_DS equ 02BH

_EBX	 equ 0
_ECX	 equ 4
_EDX	 equ 8
_ESI	 equ 0CH
_EDI	 equ 10H
_EBP	 equ 14H
_EAX	 equ 18H
_DS		 equ 1CH
_ES		 equ 20H
_O_EAX   equ 24H
_EIP     equ 28H
_CS      equ 2CH
_EFLAGS  equ 30H
_OLD_ESP equ 34H
_OLD_SS  equ 38H

%macro SAVE_ALL 0
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
	add esp, 4 ; Jump over error code / intr vector 
	iret
%endmacro

ret_from_system_call:
	RESTORE_ALL

align 4
; old_ss
; old_esp
; eflags
; cs
; eip
; already in stack
divide_error: ; Fault
	push 0 ; 0 means no error code
	SAVE_ALL
	push do_divide_error
	nop ; Patch align 4
align 4
error_code:
	; Disable hardware debugging...
	;xor eax, eax
	;mov db7, eax
	;mov eax, -1
	xchg eax, [esp + _O_EAX + 4]  ; eax = error code
	push eax
	lea edx, [esp + 8] ; preg_t address
	push edx
	mov dx, KERNEL_DS
	mov ds, dx
	mov es, dx
	; push err_code
	; push pret_t addr
	; call handler
	call [esp + 8]
	add esp, 8
	add esp, 4
	jmp ret_from_system_call


align 4
debug: ; Fault / Trap
	push 0
	SAVE_ALL
	push do_debug
	jmp error_code

align 4
nmi: ; interrupt
	push 0
	SAVE_ALL
	push do_nmi
	jmp error_code

align 4
_int3:
	push 0
	SAVE_ALL
	push do_int3
	jmp error_code

align 4
overflow: ; Trap
	push 0
	SAVE_ALL
	push do_overflow
	jmp error_code

align 4
bounds: ; Fault
	push  0
	SAVE_ALL
	push do_bounds
	jmp error_code

align 4
invalid_op: ; Fault
	push 0
	SAVE_ALL
	push do_invalid_op
	jmp error_code

align 4
device_not_available: ; Fault
	push 0
	SAVE_ALL
	push do_device_not_available
	jmp error_code

align 4
double_fault: ; Abort
	SAVE_ALL
	push do_double_fault
	jmp error_code

align 4
coprocessor_segment_overrun: ; Fault
	push 0
	SAVE_ALL
	push do_coprocessor_segment_overrun
	jmp error_code

align 4
invalid_tss: ; Fault
	SAVE_ALL
	push do_invalid_tss
	jmp error_code

align 4
segment_not_present: ; Fault
	SAVE_ALL
	push do_segment_not_present
	jmp error_code

align 4
stack_segment: ; Fault
	SAVE_ALL
	push do_stack_segment
	jmp error_code

align 4
general_protection: ; Fault
	SAVE_ALL
	push do_general_protection
	jmp error_code

align 4
page_fault: ; Fault
	SAVE_ALL
	push do_page_fault
	jmp error_code

align 4
x87_error: ; Fault
	push 0
	SAVE_ALL
	push do_x87_error
	jmp error_code

align 4
alignment_check: ; Fault
	SAVE_ALL
	push do_alignment_check
	jmp error_code

align 4
machine_check: ; Abort
	push 0
	push do_alignment_check
	jmp error_code

align 4
smid_exception: ; Fault
	push 0
	push do_smid_exception
	jmp error_code

align 4
system_call: ; Fault
	push 0
	push do_system_call
	jmp error_code
