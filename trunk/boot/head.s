%include "protect.inc"
extern user_stack
extern main
global pde

[SECTION .text]
BITS 32
global startup32
global _idt
global _gdt
global pde
startup32:

	mov ax, KERNEL_DS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, user_stack
	call init_idt

	; Copy bootup parameters
	mov esi, 090000H
	mov edi, empty_zero_page
	mov ecx, 512
	cld
	rep movsd

	call setup_paging 
	lidt [_idt_pesudo]
	lgdt [_gdt_pesudo]
	jmp KERNEL_CS:RELOAD_GDT
RELOAD_GDT:
	mov ax, KERNEL_DS
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, user_stack

	push 0
	push 0
	push 0
	call main
	jmp $

times (01000H - ($ - $$)) db 0
pde:

times (02000H - ($ - $$)) db 0
pte0:

times (03000H - ($ - $$)) db 0
pte1:

times (04000H - ($ - $$)) db 0
pte2:

times (05000H - ($ - $$)) db 0
empty_zero_page:

times (06000H - ($ - $$)) db 0


setup_paging:
	enter 0, 0
	mov ecx, 1024 * 2
	xor eax, eax
	mov edi, pde
	repne stosd

	; Initialize PDE entries
	mov dword [pde], pte0 + 7			; 0x00000000
	mov dword [pde + 3072], pte0 + 7	; 0xC0000000

	; Iinitialize PTE entries
	mov edi, pte0 + 4092
	mov eax, 3FF007H
	std
CONTINUE_INIT_PTE:
	stosd
	sub eax, 01000H
	jge CONTINUE_INIT_PTE

	mov eax, pde
	mov cr3, eax
	mov eax, cr0
	or eax, 80000000H
	mov cr0, eax
	leave
	ret

init_idt:
	enter 0, 0
	push ebx
	push ecx
	push edi
	mov eax, triple_interrupt
	mov ebx, 0800H
	mov bx, ax		; ebx: selector | low offset(low 15 bits)
	mov ax, 08E00H	; eax: high offset(high 16 bits) | attribute
	mov ecx, 256
	mov edi, _idt
INIT_IDT:
	mov [edi], ebx
	mov [edi + 4], eax
	add edi, 8
	loop INIT_IDT
	pop edi
	pop ecx
	pop ebx
	leave
	ret

init_gdt:
	lgdt [_gdt_pesudo]
	ret

int_str db "Ingoring interrupt.", 0AH
triple_interrupt:
	push ebp
	mov ebp, esp
	mov edi, int_str
	;call disp_str32
	mov esp, ebp
	pop ebp
	iret

; 256 Interrupt descriptors
_idt:
	times 512 dd 0
align 4
	dw 0
_idt_pesudo:
	dw 256 * 8 - 1
	dd _idt

; 256 GDT entries
_gdt:
_dummy: gdt_desc 0, 0, 0
_tmp:   gdt_desc 0, 0, 0
r0_code:  gdt_desc 03FFFFH, 0C0000000H, 0C09AH
r0_data:  gdt_desc 03FFFFH, 0C0000000H, 0C092H
r3_code:  gdt_desc 0BFFFFH, 0, 0C0FAH
r3_data:  gdt_desc 0BFFFFH, 0, 0C0F2H
	gdt_desc 0, 0, 0
	gdt_desc 0, 0, 0
	times (2 * 128) dd 0

NR_TASKS	equ 128

align 4
	dw 0
_gdt_pesudo:
	dw 256 * 8 - 1
	dd 0C0000000H + _gdt
