
;--------------------------------------------------
; Display AL at es:edi
disp_al:
	push ebp
	mov ebp, esp
	push ax
	push cx
	push dx
	
	mov ah, 05H
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

	mov esp, ebp
	pop ebp
	ret

;--------------------------------------------------
; Display an integer in eax
disp_int:
	push ebp
	mov ebp, esp
	
	push eax
	shr eax, 24
	call disp_al
	pop eax

	push eax
	shr eax, 16
	call disp_al
	pop eax

	push eax
	shr eax, 8
	call disp_al
	pop eax

	call disp_al

	mov esp, ebp
	pop ebp
	ret

;--------------------------------------------------
; Display a string pointed by ds:si
disp_str:
	push ebp
	mov ebp, esp
	push eax
	push esi
	
	mov ah, 05H

GO_ON_DISP:
	lodsb
	cmp al, 0
	je DISP_OVER
	cmp al, 0AH
	jne GENERAL_CHAR
	
	call disp_ret
	jmp RET_CHAR

GENERAL_CHAR:
	stosw
RET_CHAR:
	jmp GO_ON_DISP

DISP_OVER:
	
	pop esi
	pop eax
	mov esp, ebp
	pop ebp
	ret

;--------------------------------------------------
; Display an Enter
disp_ret:
	push ebp
	mov ebp, esp
	push eax
	push ebx
	push edx

	xor edx, edx
	mov eax, edi
	mov ebx, 160
	div ebx
	inc eax
	xor edx, edx
	mul ebx
	mov edi, eax

	pop edx
	pop ebx
	pop eax

	mov esp, ebp
	pop ebp
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

;-------------------------------------------------
; Memory copy
mem_cpy:
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ecx
	
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ecx, [ebp + 16]

	rep movsb

	mov eax, [ebp + 8]
	
	pop ecx
	pop esi
	pop edi
	mov esp, ebp
	pop ebp
	ret
