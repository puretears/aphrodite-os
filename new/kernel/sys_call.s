extern do_divide_error
extern do_reserved
global ret_from_system_call
global divide_error
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
divide_error:
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
reserved:
	push 0
	push do_reserved
	jmp error_code
