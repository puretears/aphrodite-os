extern do_divide_error
extern do_nmi
extern do_int3
extern do_overflow
extern do_bound_range_exceeded
extern do_invalid_opcode
extern do_device_not_available
extern do_double_fault
extern do_coprocessor_segment_overrun
extern do_invalid_tss
extern do_segment_not_present
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
global nmi
global int3
global overflow
global bound_range_exceeded
global invalid_opcode
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

section .text
; Stack layout in 'ret_from_system_call'
; +40 - old_ss
; +3C - old_esp
; +38 - eflags
; +34 - cs
; +30 - eip
; +2C - orig_eax (err_code)
; +28 - gs (exception_handler)
; +24 - fs
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
_FS		 equ 24H
_GS      equ 28H
_O_EAX   equ 2CH
_EIP     equ 30H
_CS      equ 34H
_EFLAGS  equ 38H
_OLD_ESP equ 3CH
_OLD_SS  equ 40H

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
	add esp, 4 ; Jump over handler address
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
	push do_divide_error
	nop ; Patch align 4
align 4
error_code:
	push fs
	push es
	push ds
	push eax
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	; Disable hardware debugging...
	;xor eax, eax
	;mov db7, eax
	mov eax, -1
	xchg eax, [esp + _O_EAX]  ; eax = error code
	mov bx, gs
	xchg ebx, [esp + _GS] ; Save gs and get handler address 
	push eax
	lea edx, [esp + 4] ; preg_t address
	push edx
	mov dx, KERNEL_DS
	mov ds, dx
	mov es, dx
	mov dx, USER_DS
	mov fs, dx
	; push err_code
	; push pret_t addr
	; call handler
	call ebx
	add esp, 8
	jmp ret_from_system_call


align 4
reserved: ; Fault / Trap
	push 0
	push do_reserved
	jmp error_code

align 4
nmi: ; interrupt
	push 0
	push do_nmi
	jmp error_code

align 4
int3:
	push 0
	push do_int3
	jmp error_code

align 4
overflow: ; Trap
	push 0
	push do_overflow
	jmp error_code

align 4
bound_range_exceeded: ; Fault
	push  0
	push do_bound_range_exceeded
	jmp error_code

align 4
invalid_opcode: ; Fault
	push 0
	push do_invalid_opcode
	jmp error_code

align 4
device_not_available: ; Fault
	push 0
	push do_device_not_available
	jmp error_code

align 4
double_fault: ; Abort
	push do_double_fault
	jmp error_code

align 4
coprocessor_segment_overrun: ; Fault
	push 0
	push do_coprocessor_segment_overrun
	jmp error_code

align 4
invalid_tss: ; Fault
	push do_invalid_tss
	jmp error_code

align 4
segment_not_present: ; Fault
	push do_segment_not_present
	jmp error_code

align 4
stack_segment_fault: ; Fault
	push do_stack_segment_fault
	jmp error_code

align 4
general_protection: ; Fault
	push do_general_protection
	jmp error_code

align 4
page_fault: ; Fault
	push do_page_fault
	jmp error_code

align 4
x87_error: ; Fault
	push 0
	push do_x87_error
	jmp error_code

align 4
alignment_check: ; Fault
	push do_alignment_check
	jmp error_code

;align 4
;machine_check: ; Abort
;	push 0
;	push do_alignment_check
;	jmp error_code

;align 4
;smid_exception: ; Fault
;	push 0
;	push do_smid_exception
;	jmp error_code
