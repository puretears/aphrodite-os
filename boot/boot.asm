
BOOT_ADDR	equ 09000H
SETUP_ADDR	equ 09020H
SETUP_LEN	equ 4
KERNEL_ADDR	equ 01000H
KERNEL_SIZE equ 03000H
KERNEL_END	equ KERNEL_ADDR + KERNEL_SIZE

	mov eax, [0100H]
	mov eax, [var1]
var1 dd 0
	; Copy itself from 0x7C00 to 0x9000
	mov ax, 07C0H
	mov ds, ax
	mov ax, BOOT_ADDR
	mov es, ax
	xor si, si
	xor di, di
	mov cx, 256
	rep movsw
	jmp BOOT_ADDR:GO

	; Copy setup code between 0x90200 and 0x90A00
GO:
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, 0FF0H
	call clear_scr

	mov ax, SETUP_ADDR
	mov es, ax
	mov dx, 0
	mov ch, 0
	mov cl, 2
	mov ah, 2
	mov al, SETUP_LEN
	xor bx, bx
	int 13H
	jc GO

	; Copy system module at 0x10000
GET_DISK_PARA:
	xor dl, dl
	mov ah, 08H
	int 13H
	jc GET_DISK_PARA
	mov byte [secs_per_track], cl
	
	mov ax, KERNEL_ADDR
	mov es, ax
	xor bx, bx
CONTINUE_READ:
	mov ax, es
	cmp ax, KERNEL_END
	jnz CONTINUE_LOAD_KERNEL

	call kill_motor
	jmp SETUP_ADDR:0 ; Jump to the SETUP code!

CONTINUE_LOAD_KERNEL:
	movzx ax, byte [secs_per_track]
	sub ax, [total_read_secs]
	mov [curr_read_secs], ax
	xor dx, dx
	mov cx, 512
	mul cx
	add ax, bx
	jnc READ1
	je READ1
	xor cx, cx
	sub cx, bx
	shr cx, 9
	mov [curr_read_secs], cx
READ1:
	push word [curr_read_secs]
	call read_track
	mov ax, [curr_read_secs]
	add ax, [total_read_secs]
	mov [total_read_secs], ax
	cmp al, [secs_per_track]
	jnz READ2

	mov al, 1
	sub al, [curr_head]
	jnz CHANGE21
	inc byte [curr_track]
CHANGE21:
	mov [curr_head], al
	mov word [total_read_secs], 0

READ2:
	mov cx, [curr_read_secs]
	shl cx, 9
	add bx, cx
	jnc CONTINUE_READ
	mov ax, es
	add ax, 01000H
	mov es, ax
	xor bx, bx
	jmp CONTINUE_READ


read_track:
	enter 0, 0
	pusha
	mov dh, [curr_head]
	mov dl, 0
	mov ch, [curr_track]
	mov cl, byte [total_read_secs]
	inc cl
	mov ah, 02H
	mov al, byte [bp + 4]
	int 13H
	jc $	; Hung here if error!!!
	popa
	leave
	ret

kill_motor:
	push dx
	mov dx, 03F2H
	mov al, 0
	out dx, al
	pop dx
	ret
%include "display.inc"
curr_head		db 0
curr_track		db 0
secs_per_track	db 0
curr_read_secs	dw 0
curr_pos		dw 0
total_read_secs	dw 5
	times (510 - ($ - $$)) db 0
	dw 0xAA55
