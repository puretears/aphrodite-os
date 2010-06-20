; Display a string pointed by ds:si
disp_str:
	enter 0, 0
	push es
	push si
	push di
	push dx

	mov ax, 0B800H
	mov es, ax
	mov di, [curr_pos] 
	mov ah, 0CH
GO_ON_DISP:
	lodsb
	cmp al, 0
	jz DISP_OVER
	cmp al, 0AH
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

	pop dx
	pop di
	pop si
	pop es
	leave 
	ret

disp_newline:
	enter 0, 0
	push dx
	push di
	mov ax, [curr_pos]
	xor dx, dx
	mov di, 160
	div di
	inc ax
	xor dx, dx
	mov di, 160
	mul di
	mov [curr_pos], ax
	pop di
	pop dx
	leave
	ret

; push int
; call disp_int
disp_int:
	enter 0, 0
	push es
	push ecx
	mov eax, dword [bp + 4] 
	xor ecx, ecx
	mov cx, 32
DISP_BITS:
	push eax
	sub cx, 4
	shr eax, cl
	and eax, 0FH
	call disp_al
	pop eax
	cmp cx, 0
	jnz DISP_BITS
	pop ecx
	pop es
	leave
	ret

disp_al:
	enter 0, 0
	push es
	push di
	push bx
	mov bx, 0B800H
	mov es, bx
	mov di, [curr_pos]
	cmp al, 9
	jg A2F
	add al, 0x30
	jmp DISP_AL1
A2F:
	add al, 0x37
DISP_AL1:
	mov ah, 0CH
	stosw
	mov [curr_pos], di
	pop bx
	pop di
	pop es
	leave
	ret

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
