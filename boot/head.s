%include "../boot/protect.inc"
extern user_stack
extern main
global pde

[SECTION .text]
BITS 32
pde:	; Page directory will be stored here
global startup32
global _idt
global _gdt
global pde
startup32:

	mov ax, 010H
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, user_stack
	call init_idt
	call init_gdt
	mov ax, 010H
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, user_stack
	jmp code_after_pg

times (01000H - ($ - $$)) db 0
pte0:

times (02000H - ($ - $$)) db 0
pte1:

times (03000H - ($ - $$)) db 0
pte2:

times (04000H - ($ - $$)) db 0
pte3:

times (05000H - ($ - $$)) db 0

code_after_pg:
	push 0
	push 0
	push 0
	push HANG_HERE
	push main
	jmp setup_paging
HANG_HERE:
	jmp $

setup_paging:
	enter 0, 0
	; Initialize PDE entries
	mov dword [pde], pte0 + 7
	mov dword [pde + 4], pte1 + 7
	mov dword [pde + 8], pte2 + 7
	mov dword [pde + 0CH], pte3 + 7
	; Initialize PTE entries
	mov ecx, 4096
	mov eax, 7
	mov edi, pte0
CONTINUE_INIT_PTE:
	stosd
	add eax, 1000H
	loop CONTINUE_INIT_PTE
	xor eax, eax
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
	mov bx, ax	; ebx: selector | low offset(low 15 bits)
	mov ax, 08E00H ; eax: high offset(high 16 bits) | attribute
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
	lidt [_idt_pesudo]
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
_code:  gdt_desc 0FFFFFH, 0, 0C9AH
_data:  gdt_desc 0FFFFFH, 0, 0C92H
_tmp:   gdt_desc 0, 0, 0
	times 504 dd 0

align 4
	dw 0
_gdt_pesudo:
	dw 256 * 8 - 1
	dd _gdt
