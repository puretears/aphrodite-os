extern do_divede_error

[section .text]
bits  32
export divide_error


divide_error:
	push do_divide_error
	jmp no_error_code
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
	mov dx, 0x10
	mov ds, dx
	mov es, dx
	mov fs, dx
	push 0
	lea edx, [esp + 44]
	push edx ; Exception handler address
	call dword [eax]
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
