%include	"load.inc"
	mov eax,[0100H]
	mov eax, [var1]
var1 dd 0

	mov ax, 07c0H
	mov ds, ax
	mov ax, BOOT_ADDR
	mov es, ax
	xor si, si
	xor di, di
	mov cx, 256
	rep movsw 	; ds:si->es:di 
	jmp BOOT_ADDR:GO

;Copy setup code to between 0x90200 and 0x90a00
GO:	
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, 0ff0h
	call clear_scr
	mov si, boot_copy
	call disp_str	

	; read floopy dirve to es:bx
	mov ax, SETUP_ADDR
	mov es, ax
	xor bx, bx
	mov ah, 2	;read
	mov al, SETUP_LEN ; count of sektor
	mov cl, 2	; sektor
	mov ch, 0	; magnetkopf
	mov dl, 0	; 0 driver
	int 13H
	jc DISP_ERROR
	jmp GET_DISK_PARA
DISP_ERROR:
	mov si, copy_error
	call disp_str
	
GET_DISK_PARA:
	xor dl, dl
	mov ah, 08H
	int 13H
	jc GET_DISK_PARA
	mov byte [secs_per_track], cl   ; 扇区数
	
	mov ax, KERNEL_ADDR
	mov es, ax
	xor bx, bx
READ:
	mov ax, es
	cmp ax, KERNEL_END
	jnz CONTINU_LOAD_KERNEL
	
	call kill_motor
	mov si, read_kernel
	call disp_str
	jmp SETUP_ADDR:0
	;Copy system module at 0x10000

CONTINU_LOAD_KERNEL:
	movzx ax, byte [secs_per_track]	;ax 高位为0填充
	sub ax, [total_read_secs]	;一共有多少扇区要读
	mov [curr_read_secs], ax	;当前读的扇区
	xor dx, dx
	mov cx, 512
	mul cx
	add ax, bx
	jnc READ1
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
	jnc READ
	mov ax, es
	add ax, 01000H
	mov es, ax
	xor bx, bx
	jmp READ
	
kill_motor:
	enter 0,0
	push dx
	mov dx, 03F2H
	mov al, 0
	out dx, al
	pop dx
	leave
	ret
	
read_track:
	enter 0, 0
	pusha
	mov ah, 02H ;功能号
	mov al, byte [bp + 4]	;扇区数
	mov ch, [curr_track]	;柱面
	mov cl, byte [total_read_secs]	;扇区
	inc cl, 
	mov dh, [curr_head]	;磁头
	mov dl, 0
	int 13H 
	popa
	leave
	ret
		
%include 	"head.inc"
boot_copy	db "copy bootsect to 0x90000", 0AH, 0
copy_error	db "copy bootsect to 0x90000 error", 0AH, 0
read_kernel	db "read kernel"
secs_per_track	db 0
total_read_secs dw 5
curr_read_secs	dw 0
curr_pos	dw 0
curr_head 	db 0
curr_track	db 0
	times (510 - ($-$$)) db 0
	dw 0xAA55
