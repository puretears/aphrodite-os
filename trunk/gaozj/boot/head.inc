clear_scr:
	enter 0, 0
	push es
	push di
	push cx
	mov ax, 0B800H
	mov es, ax
	xor di, di
	mov ah, 00
	mov al, 20H
	mov cx, 2000
	rep stosw
	pop cx
	pop di
	pop es
	leave
	ret


disp_str:
	enter 0,0
	pusha
	mov ax, 0B800H
	mov es, ax
	mov di, [curr_pos]
	mov ah, 0FH
GO_ON_DISP:
	lodsb
	cmp al, 0 
	jz DISP_OVER
	cmp al, 0AH ;  是否为回车
	jnz DISP_CHAR
	xor dx, dx
	mov ax, di
	mov di, 160
	div di
	inc ax
	xor dx, dx 
	mov di, 160
	mul di 
	mov di, ax
	jmp GO_ON_DISP
DISP_CHAR:
	stosw
	jmp GO_ON_DISP
DISP_OVER: 
	mov [curr_pos], di
	 	
	popa
	leave 
	ret 
