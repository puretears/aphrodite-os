%include "sconst.inc"

KERNEL_SEL	equ 8

extern cstart
extern exception_handler
extern spurious_irq
extern gdt_ptr
extern idt_ptr
extern disp_str
extern curr_pos
extern aphrodite_main
extern p_curr_proc
extern tss0
extern delay
extern int_re_enter
extern clock_handler

[section .bss]
LABEL_KERNEL_STACK resb 2 * 1024
TOP_OF_STACK:


[section .data]
	clock_int_msg db "^", 0

[section .text]
global _start
global restart
; Exception and interrupt referrence
global divide_error
global single_step_error
global nmi_interrupt
global breakpoint
global overflow
global bound_range_exceeded
global invalid_opcode
global device_not_available
global double_fault
global coprocessor_segment_overrun
global invalid_tss
global segment_not_present
global stack_fault
global general_protection
global page_fault
global x87_fpu_floating_point_error
; The following exceptions are not implemented yet.
;global alignment_check
;global machine_check
;global simd_floating_point
;---------------Interrupt from 8259A-----------------
global hw_irq00
global hw_irq01
global hw_irq02
global hw_irq03
global hw_irq04
global hw_irq05
global hw_irq06
global hw_irq07
global hw_irq08
global hw_irq09
global hw_irq10
global hw_irq11
global hw_irq12
global hw_irq13
global hw_irq14
global hw_irq15

_start:
	mov esp, TOP_OF_STACK
	mov dword [curr_pos], 0
	sgdt [gdt_ptr]
	call cstart
	lgdt [gdt_ptr]
	lidt [idt_ptr]
	jmp KERNEL_SEL:csinit

csinit: 
	;mov eax, 1
	;cpuid
	;test edx, 0x00000100
	;jnz APIC_EXIST
	;push str_apic_noexist
	;call disp_str
	;add esp, 4
	;jmp DEBUG_TEST
APIC_EXIST:
	;push str_apic_exist
	;call disp_str
	;add esp, 4
DEBUG_TEST:
	;ud2
	
	;sti
	xor eax, eax
	mov ax, TSS_SEL_ASM
	ltr ax
	jmp aphrodite_main
	;hlt

str_apic_exist db "APIC existed!", 0AH, 0
str_apic_noexist db "APIC doesn't exist!", 0AH, 0

restart:
	mov esp, [p_curr_proc]
	lldt word [esp + PROCESS_LDT_SEL]
	lea eax, [esp + TOP_OF_PROC_STACK]
	mov dword [tss0 + TSS_ESP0], eax
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 4
	iretd


; Hardware interrupt
%macro hw_irq_handler 1
	push 1
	call spurious_irq
	add esp, 4
	hlt
%endmacro

ALIGN 16
; Time interrupt
hw_irq00:
	; When executing here, CPU had already pushed 
	; ss
	; esp
	; eflags
	; cs
	; eip
	; into ring 0 stack which is read from TSS.
	; So we need to jump over 4 bytes of retaddr field of stack_frame.
	sub esp, 4
	pushad
	push ds
	push es
	push fs
	push gs
	
	; Use kernel self data segments
	mov dx, ss
	mov ds, dx
	mov es, dx
	inc byte [gs:0]
	mov al, EOI
	out INT_MASTER1, al

	inc dword [int_re_enter]
	cmp dword [int_re_enter], 0
	jnz INT_RE_ENTER	
	
	; Switch to kernel-self stack
	mov esp, TOP_OF_STACK	
	sti

	;push clock_int_msg
	;call disp_str
	;add esp, 4
	push 0
	call clock_handler
	add esp, 4
	
	cli
	; Switch to kernel process storing stack
	mov esp, [p_curr_proc]
	lldt [esp + PROCESS_LDT_SEL]
	lea eax, [esp + TOP_OF_PROC_STACK]
	mov [tss0 + TSS_ESP0], eax
	; Restore the new process state
INT_RE_ENTER:
	dec dword [int_re_enter]
	pop gs
	pop fs
	pop es
	pop ds
	popad
	add esp, 4
	iretd

ALIGN 16
hw_irq01:
	hw_irq_handler 1

ALIGN 16
hw_irq02:
	hw_irq_handler 2

ALIGN 16
hw_irq03:
	hw_irq_handler 3

ALIGN 16
hw_irq04:
	hw_irq_handler 4

ALIGN 16
hw_irq05:
	hw_irq_handler 5

ALIGN 16
hw_irq06:
	hw_irq_handler 6

ALIGN 16
hw_irq07:
	hw_irq_handler 7

ALIGN 16
hw_irq08:
	hw_irq_handler 8

ALIGN 16
hw_irq09:
	hw_irq_handler 9

ALIGN 16
hw_irq10:
	hw_irq_handler 10

ALIGN 16
hw_irq11:
	hw_irq_handler 11

ALIGN 16
hw_irq12:
	hw_irq_handler 12

ALIGN 16
hw_irq13:
	hw_irq_handler 13

ALIGN 16
hw_irq14:
	hw_irq_handler 14

ALIGN 16
hw_irq15:
	hw_irq_handler 15

ALIGN 16
hw_irq16:
	hw_irq_handler 16

; Exception and interrupt handling
divide_error:
	push 0xFFFFFFFF
	push 0
	jmp exception
single_step_error:
	push 0xFFFFFFFF
	push 1
	jmp exception
nmi_interrupt:
	push 0xFFFFFFFF
	push 2
	jmp exception
breakpoint:
	push 0xFFFFFFFF
	push 3
	jmp exception
overflow:
	push 0xFFFFFFFF
	push 4
	jmp exception
bound_range_exceeded:
	push 0xFFFFFFFF
	push 5
	jmp exception
invalid_opcode:
	push 0xFFFFFFFF
	push 6
	jmp exception
device_not_available:
	push 0xFFFFFFFF
	push 7
	jmp exception
double_fault:
	push 8
	jmp exception
coprocessor_segment_overrun:
	push 0xFFFFFFFF
	push 9
	jmp exception
invalid_tss:
	push 10
	jmp exception
segment_not_present:
	push 11
	jmp exception
stack_fault:
	push 12
	jmp exception
general_protection:
	push 13
	jmp exception
page_fault:
	push 14
	jmp exception
x87_fpu_floating_point_error:
	push 0xFFFFFFFF
	push 16
	jmp exception

exception:
	call exception_handler
	add esp, 8
	hlt



