global cur_pos

BOOT_ADDR	equ 09000H
LOADER_ADDR	equ 09020H
KERNEL_ADDR equ 01000H

	mov ax, BOOT_ADDR
	mov ds, ax
	mov ax, LOADER_ADDR
	mov es, ax

	mov ah, 03H
	xor bh, bh
	int 10H
	mov [0], dx			; Save current cursor position at 0x90000
	
	call get_mem_map
	call disp_mem_map
	mov eax, dword [mem_size_low]
	mov [4], eax
	mov eax, dword [mem_size_high]
	mov [8], eax
	

	mov ah, 0FH
	int 10H
	mov [12], bx			; bh = current page
	mov [14], ax			; ah = window width, al = video mode

	mov ah, 12H
	mov bl, 10H
	int 10H
	mov [16], ax
	mov [18], bx		; bh = display mode, bl = display memory
	mov [20], cx		; cx = video card parameter

	; Load the 1st disk parameter into 0x90080
	mov ax, 0
	mov ds, ax
	lds si, [41 * 4]
	mov ax, BOOT_ADDR
	mov es, ax
	mov di, 080H
	mov cx, 10H
	rep movsb

	cli
	cld
	; Move system module from 0x10000 to 0x00000
	xor ax, ax
CONTINUE_COPY:
	mov es, ax
	add ax, 01000H
	cmp ax, 09000H
	jz COPY_OVER
	mov ds, ax
	xor si, si
	xor di, di
	mov cx, 08000H
	rep movsw
	jmp CONTINUE_COPY
COPY_OVER:

	mov ax, cs
	mov ds, ax
	mov si, loader
	call disp_str

	call enableA20
	call init8259A

	lgdt [gdt_pesudo]
	;lidt [idt_pesudo]
	; Jump to protect mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword CODE32_SEL:0


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

	mov eax, 0E820H
	mov ebx, 0
	mov ecx, 512
	mov edx, 0534D4150H
	mov edi, _mem_map_buffer
INT15H:
	int 15H
	jnc CONTINUE_TO_SCAN
	jmp $		; If error, just hang here just for simplicity.
CONTINUE_TO_SCAN:
	cmp ebx, 0
	jz SCAN_END
	add edi, ecx
	mov eax, 0E820H
	jmp INT15H
SCAN_END:
	; Now we have get the whole mem map, EDI points to the last entry.
	; mov eax, dword [edi]
	; mov edx, dword [edi + 4]
	; add eax, dword [edi + 8]
	; add edx, dword [edi + 12]
	; mov dword [mem_size_low], eax
	; mov dword [mem_size_high], edx

	pop edi
	pop edx
	pop ecx
	pop ebx
	leave
	ret

disp_mem_map:
	enter 0, 0
	push ds
	push esi
	mov ax, LOADER_ADDR
	mov ds, ax
	lea esi, [_mem_map_buffer + 40]	
	lodsd
	push eax
	call disp_int
	add sp, 4
	pop esi
	pop ds
	leave
	ret
%include "display.inc"
%include "protect.inc"
loader		db "Load loader to 0x90200.", 0AH, 0
curr_pos	dw 160


gdt: gdt_desc 0, 0, 0
code: gdt_desc 0FFFFH, 0, 0C09AH	; Code segment
data: gdt_desc 0FFFFH, 0, 0C092H 

CODE32_SEL equ code - gdt

gdt_pesudo dw $ - gdt
		   dd 090200H + gdt

;idt_pesudo dw 0
;           dd 0


_mem_map_buffer times 512 db 0
mem_size_low dd 0
mem_size_high dd 0
