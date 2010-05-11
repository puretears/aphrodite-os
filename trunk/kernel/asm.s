extern do_divede_error

[section .text]
bits  32
export divide_error
export nmi
export breakpoint
export overflow
export bounds
export invalid_opcode
export device_not_avaiable
export coprocessor_segment_overrun
export x87_error 
export machine_check
export smid_exception
export reserved
export double_fault
export invalid_tss
export segment_not_present
export stack_seg_fault
export general_protect
export page_fault
export alignment_check

divide_error:
	push do_divide_error
no_error_code:
	xchg eax, dword [esp]
	push ebx
	push ecx
	push edx
	push esi
	push edi 
	push ebp
	push ds
	push es
	push fs
	mov dx, 010H
	mov ds, dx
	mov es, dx
	mov fs, dx
	push 0
	lea edx, [esp + 44]
	push edx
	call dword [edx]
	add esp, 8
	pop fs
	pop es
	pop ds
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret

nmi:
	push do_nmi
	jmp no_error_code

breakpoint:
	push do_int3
	jmp no_error_code

overflow:
	push do_overflow
	jmp no_error_code

bounds: 
	push do_bound_range_exceeded
	jmp no_error_code

invalid_opcode:
	push do_invalid_opcode
	jmp no_error_code

device_not_avaiable:
	push do_no_coprocessor
	jmp no_error_code

coprocessor_segment_overrun:
	push do_cop_seg_overrun
	jmp no_error_code

x87_error:
	push do_x87_error
	jmp no_error_code

machine_check:
	push do_machine_check
	jmp no_error_code

smid_exception:
	push do_smid_exception
	jmp no_error_code

reserved:
	push do_reserved
	jmp no_error_code

; push ss
; push esp
; push eflags
; push cs
; push eip
; push error_code

double_fault:
	push do_double_fault
error_code:
	xchg ebx, dword [esp]		; Handler address	
	xchg eax, dword [esp + 4]	; Error code
	push ecx
	push edx
	push esi
	push edi
	push ebp
	push ds
	push es
	push fs
	mov ax, 010H
	mov ds, ax
	mov es, ax
	mov fs, ax
	push eax
	lea eax, dword [esp + 44]
	push eax
	call dword [ebx]
	add esp, 8
	pop fs
	pop es
	pop ds
	pop ebp
	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx
	pop eax
	iret

invalid_tss:
	push do_invalid_tss
	jmp error_code
	
segment_not_present:
	push do_seg_not_present
	jmp error_code

stack_seg_fault:
	push do_stack_seg_fault
	jmp error_code

general_protect:
	push do_general_protect
	jmp error_code

page_fault:
	push do_page_fault
	jmp error_code

alignment_check:
	push do_alignment_check
	jmp error_code
