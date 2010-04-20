BOOT_ADDR	equ 09000H
LOADER_ADDR	equ 09020H
KERNEL_ADDR equ 01000H

	mov ax, BOOT_ADDR
	mov ds, ax
	mov ah, 03H
	xor bh, bh
	int 10H
	mov [0], dx			; Save current cursor position at 0x90000
	
	mov ah, 088H
	int 015H
	mov [2], ax			; Save extend memory(above 1MB) at 0x90002 
	
	mov ah, 0FH
	int 10H
	mov [4], bx			; bh = current page
	mov [6], ax			; ah = window width, al = video mode

	mov ah, 12H
	mov bl, 10H
	int 10H
	mov [8], ax
	mov [0AH], bx		; bh = display mode, bl = display memory
	mov [0BH], cx		; cx = video card parameter

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
	lidt [idt_pesudo]
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
	mov al, 0FFH
	out 21H, al
	out 0A1H, al
	ret
%include "display.inc"
%include "protect.inc"
loader		db "Load loader to 0x90200.", 0AH, 0
curr_pos	dw 160


gdt: gdt_desc 0, 0, 0
code: gdt_desc 0799H, 0, 0C09AH	; Code segment
data: gdt_desc 0799H, 0, 0C092H 

CODE32_SEL equ code - gdt

gdt_pesudo dw $ - gdt
		   dd 090200H + gdt

idt_pesudo dw 0
           dd 0
