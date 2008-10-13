	org 7C00H


	jmp short LABEL_START
	nop

	%include "fat12hdr.inc"
	%include "load.inc"

LABEL_START:

	; bootstrap code here
	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, 0100H

	call clear_scr

	xor ah, ah
	xor dl, dl
	int 13H
	
	mov ax, LOADER_BASE
	mov es, ax
	mov ax, 19

READ_NEXT_SECTOR2:
	cmp word [rootdir_sectors], 0
	jz LABEL_NO_LOADER
	dec word [rootdir_sectors]

	push ax
	; read from root directory
	mov bx, LOADER_OFFSET
	mov cl, 1
	call read_sector

	mov cx, 16
	mov di, LOADER_OFFSET

LABEL_FIND_NEXT_ENTRY2:
	mov bx, 11
	mov si, loader_name
CONTINUE_MATCHING:
	lodsb
	cmp al, [es:di]
	jnz LABEL_FIND_NEXT_ENTRY1
	dec bx
	cmp bx, 0
	jz STRING_FOUND
	inc di
	jmp CONTINUE_MATCHING

LABEL_FIND_NEXT_ENTRY1:
	dec cx
	jcxz READ_NEXT_SECTOR1
	and di, 0FFE0H
	add di, 32
	jmp LABEL_FIND_NEXT_ENTRY2
READ_NEXT_SECTOR1:
	pop ax
	inc ax
	jmp READ_NEXT_SECTOR2

LABEL_NO_LOADER:
	mov ax, 0B800H
	mov es, ax
	mov di, 80 * 10
	mov si, no_loader
	call disp_str
	jmp $

STRING_FOUND:
	and di, 0FFE0H
	add di, 26	; offset of beginning sector
	mov ax, [es:di]

	mov bx, LOADER_OFFSET

	push es
	push ax
	push si
	mov ax, 0B800H
	mov es, ax
	mov si, boot_str
	xor di, di
	call disp_str
	pop si
	pop ax
	pop es

GO_ON_LOADING:
	push es
	push ax
	mov ax, 0B800H
	mov es, ax
	mov ah, 08H
	mov al, '.'
	mov [es:di], ax
	add di, 2
	pop ax
	pop es

	push ax		; save this beginning sector
	add ax, 14
	add ax, 17
	mov cl, 1
	call read_sector
	
	pop ax
	call get_fat_entry
	cmp ax, 0FFFH
	jz FINISH_LOADING
	add bx, 200H
	jmp GO_ON_LOADING

FINISH_LOADING:	 		
	jmp LOADER_BASE:LOADER_OFFSET

rootdir_sectors	dw ROOTDIR_SECTORS
loader_name	db "LOADER  BIN", 0
bOdd		db 0
boot_str	db "Booting", 0
no_loader	db "No loader", 0

	

; es:bx: points to buffer
; cl: number of sectors to read
; ax: starting sector number	
read_sector:
	push bp
	mov bp, sp
	sub sp, 8
	push cx
	push dx
	push bx

	xor dx, dx
	mov bx, 18
	div bx
	mov [bp - 2], ax	; track
	mov [bp - 4], ax	; head
	mov [bp - 6], dx	; sector

	shr byte [bp - 2], 1	; cylinder
	and byte [bp - 4], 1	; head
	add byte [bp - 6], 1	; sector
	
	pop bx
	mov ah, 02H
	mov al, cl
	mov ch, [bp - 2]
	mov cl, [bp - 6]
	mov dh, [bp - 4]
	mov dl, 0
	int 13H
	
	pop dx
	pop cx

	mov sp, bp
	pop bp
	ret

; ax: sector number		
get_fat_entry:
	push bp
	mov bp, sp
	push dx
	push bx
	push cx
	push es

	
	mov dx, LOADER_BASE
	sub dx, 1000H
	mov es, dx

	xor dx, dx
	mov bx, 3
	mul bx
	mov bx, 2	; ax * 3 / 2
	div bx

	cmp dx, 0
	jz LABEL_EVEN
	mov byte [bOdd], 1

LABEL_EVEN:	
	xor dx, dx
	mov bx, 512
	div bx
	push dx		; save the remainder

	add ax, 1	; 1 beginning sector of fat table
	xor bx, bx
	mov cl, 2
	call read_sector

	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [bOdd], 1
	jnz .1
	shr ax, 4
.1:
	and ax, 0FFFH
LABEL_RET:
	mov byte [bOdd], 0 ; Restore the bOdd byte!!!!!
	pop es
	pop cx
	pop bx
	pop dx
	mov sp, bp
	pop bp
	ret

disp_al:
	push bp
	mov bp, sp
	push ax
	push cx
	push dx
	
	mov ah, 08H
	mov dl, al
	mov cx, 2

	; Display high 4 bits of al
	shr al, 4
.2:
	and al, 0FH

	cmp al, 9
	jg .1
	add al, 30H
	jmp .disp

.1:
	sub al, 0AH
	add al, 'A';
.disp:
	mov word [es:di], ax
	add di, 2
	mov al, dl
	loop .2

	pop dx
	pop cx
	pop ax

	mov sp, bp
	pop bp
	ret

;--------------------------------------------------
; Display a string pointed by ds:si
disp_str:
	push bp
	mov bp, sp
	push ax
	push si

	mov ah, 08H

GO_ON_DISP:
	lodsb
	cmp al, 0
	je DISP_OVER

	stosw
RET_CHAR:
	jmp GO_ON_DISP

DISP_OVER:
	
	pop si
	pop ax
	mov sp, bp
	pop bp
	ret


;--------------------------------------------------
; Clear the screen (80 x 25)
clear_scr:
	push bp
	mov bp, sp
	push es
	push di

	mov ax, 0B800H
	mov es, ax
	xor di, di
	mov ah, 05H
	mov al, ' '
	mov cx, 80 * 25
	
	rep stosw

	pop di
	pop es
	mov sp, bp
	pop bp
	ret
times 510 - ($ - $$) db 0
	dw 0AA55H
