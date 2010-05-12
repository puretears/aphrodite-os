nr_system_calls equ 72
CS_REG			equ 44
OLD_SS			equ 56 
extern sys_call_table

export system
[section .text]
bits 32
bad_call:
	mov eax, -1
	iret
; Stack sequence
; eax 0
; ebx 4
; ecx 8
; edx 12
; esi 16
; edi 20
; ebp 24
; fs  28
; es  32
; ds  36
; eip 40
; cs  44
; ef  48
; esp 52
; ss  56
system_call:
	cmp nr_system_calls, eax
	ja bad_call
	push ds
	push es
	push fs
	push ebp
	push edi
	push esi
	push edx
	push ecx
	push ebx
	mov eax, 010H
	mov ds, ax
	mov es, ax
	mov eax, 0x17
	mov fs, ax	; fs points to the local descriptor
	call dword [sys_call_table + eax * 4]
	push eax
	; Reschedule work left here!!!
	pop eax
	pop ebx
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp
	pop fs
	pop es
	pop ds
	iret
