global cur_pos
global total_mem_size

BOOT_ADDR equ 09000H
LOADER_ADDR	equ 09020H
KERNEL_ADDR	equ 01000H
	
	mov ax, BOOT_ADDR
	mov ds, ax
	mov ax, LOADER_ADDR
	mov es, ax
	 
	mov ah, 03H
	xor bh, bh
	int 10H
	mov [0], dx	;dl=Y 		dh=X
			;save current cursor postion at 0x9000
	
	xor bx, bx
	mov ah, 0Fh
	int 10H
	mov [12], bx
	mov [14], ax

	mov ah, 12H
	mov bl, 10H
	int 10H
	mov [16], ax
	mov [18], bx
	mov [10], cx
	
	mov ax, 0
	mov ds, ax
	lds si, [41 * 4]	; offset to si
	mov ax, BOOT_ADDR
	mov es, ax
	mov di, 80
	mov cx, 10H
	rep movsb
	

	mov ax, cs
	mov ds, ax
	call get_mem_map
	
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
		

	call enableA20
	call init8259A
	lgdt [gdt_pesudo]
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword CODE32_SEL:0	;load kernel to 0x00000H	

enableA20:
	in al, 92H
	or al, 00000010B
	out 92H, al
	ret

init8259A:
	; Init ICW1
	mov al, 011H
	out 20H, al
	out 0A0H, al
	; Init ICW2
	mov al, 020H
	out 21H, al
	mov al, 28H
	out 0A1H, al
	; Init ICW3
	mov al, 4H
	out 21H, al
	mov al, 2H
	out 0A1H, al
	; Init ICW4
	mov al, 1
	out 21H, al
	out 0A1H, al
	; Prohibit all interrupts
	mov al, 0FFH
	out 21H, al
	out 0A1H, al
	ret

get_mem_map:
	enter 0, 0
	push ebx
	push ecx
	push edx
	push edi
	push esi
	xor ebx, ebx
	mov di, mem_map_buffer
GET_MEM:
	mov eax, 0E820H
	mov ecx, 20
	mov edx, 0534D4150H	
	int 15H
	jnc CONTINU_GET_MEM_MAP
	mov dword [mem_count], 0
	jmp $
CONTINU_GET_MEM_MAP:
	inc dword [mem_count]
	cmp ebx, 0
	jz GET_MEM_MAP_END
	add edi, 20
	jmp GET_MEM
GET_MEM_MAP_END:
	xor edx, edx
	mov eax, 20
	mov ecx, [mem_count]
	mul ecx
	add eax, mem_map_buffer
	mov edi, eax
	sub edi, 20
	mov ecx, [mem_count]
CONTINUE_FIND_LAST_ENTRY:
	lea esi, [edi + 16]
	lodsd
	cmp eax, 1
	jz LAST_FOUND
	sub edi, 20
	loop CONTINUE_FIND_LAST_ENTRY
LAST_FOUND:
	mov eax, [edi]		; baseL
	mov edx, [edi + 8]	; LimitL
	add eax, edx
	mov dword [total_mem], eax
	pop esi
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
total_mem	dd 0
mem_count	db	0

