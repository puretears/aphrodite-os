BOOT_ADDR equ 09000H
LOADER_ADDR	equ 09020H
KERNEL_ADDR	equ 01000H
	
	mov ax, BOOT_ADDR
	mov ds, ax
	mov ax, LOADER_ADDR
	mov es, ax
	 
	;move 0x10000 to 0x0
	xor ax, ax
MOVING:
	mov es, ax
	add ax, 01000H
	cmp ax, 09000H
	jz MOVE_END
	mov ds, ax
	xor si, si
	xor di, di
	mov cx, 08000H
	rep movsw
	jmp MOVING
MOVE_END:
		
	mov ax, cs
	mov ds, ax
	
	call get_mem_map
	mov eax, dword [mem_size_low]
	mov [4], eax
	mov eax, dword [mem_size_high]
	mov [8], eax

	call enableA20
	lgdt [gdt_pesudo]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword CODE32_SEL:0	

enableA20:
	in al, 92H
	or al, 00000010B
	out 92H, al
	ret
	
get_mem_map:
	enter 0, 0
	push ebx
	push ecx
	push edx
	push edi
	mov edx, 0534d4150H
	xor ebx, ebx
	mov di, mem_map_buffer
GET_MEM:
	mov eax, 0E820H
	mov ecx, 20
	int 15H
	jnc CONTINU_GET_MEM_MAP
	jmp $
CONTINU_GET_MEM_MAP:
	cmp ebx, 0
	jz GET_MEM_MAP_END
	add edi, ecx
	jmp GET_MEM
GET_MEM_MAP_END:
	mov eax, dword [edi]
	mov edx, dword [edi + 4]
	add eax, dword [edi + 8]
	add edx, dword [edi + 12]
	mov dword [mem_size_low], eax
	mov dword [mem_size_high], edx
	pop edi
	pop edx
	pop ecx
	pop ebx
	leave 
	ret

;gdt_sec Limit, Base_address, Attrebute
%include "protect.inc"
gdt:	gdt_desc 0, 0, 0
code: gdt_desc 0FFFFH, 0, 0C09AH	; Code segment
data: gdt_desc 0FFFFH, 0, 0C092H

CODE32_SEL	equ code - gdt	;代码选择子
gdt_pesudo	dw $ -gdt
		dd 090200H + gdt
mem_map_buffer	times	256 db 0
mem_size_low	dd	0
mem_size_high	dd	0

