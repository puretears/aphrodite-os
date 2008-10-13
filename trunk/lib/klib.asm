
extern curr_pos

[section .text]
global disp_str
global disp_color_str
global write_to_port
global read_from_port

disp_str:
	push ebp
	mov ebp, esp

	mov esi, [ebp + 8]
	mov edi, dword [curr_pos]
	mov ah, 0FH

LABEL_LOAD_STRING:
	lodsb
	cmp al, 0
	jz LABEL_DISP_OVER
	cmp al, 0AH
	jnz LABEL_DISP_CHAR
	push eax
	xor edx, edx
	mov eax, edi
	mov ebx, 160
	div ebx
	and eax, 0FFH
	xor edx, edx
	inc eax
	mov ebx, 160
	mul ebx
	mov edi, eax
	pop eax
	jmp LABEL_LOAD_STRING

LABEL_DISP_CHAR:
	mov [gs:edi], ax
	add edi, 2
	jmp LABEL_LOAD_STRING

LABEL_DISP_OVER:
	mov [curr_pos], edi	
	mov esp, ebp
	pop ebp
	ret

disp_color_str:
	push ebp
	mov ebp, esp

	mov esi, [ebp + 8]
	mov edi, dword [curr_pos]
	mov ah, [ebp + 12]

LABEL_LOAD_COLOR_STRING:
	lodsb
	cmp al, 0
	jz LABEL_DISP_COLOR_OVER
	cmp al, 0AH
	jnz LABEL_DISP_COLOR_CHAR
	push eax
	xor edx, edx
	mov eax, edi
	mov ebx, 160
	div ebx
	and eax, 0FFH
	xor edx, edx
	inc eax
	mov ebx, 160
	mul ebx
	mov edi, eax
	pop eax
	jmp LABEL_LOAD_COLOR_STRING

LABEL_DISP_COLOR_CHAR:
	mov [gs:edi], ax
	add edi, 2
	jmp LABEL_LOAD_COLOR_STRING

LABEL_DISP_COLOR_OVER:
	mov [curr_pos], edi	
	mov esp, ebp
	pop ebp
	ret


write_to_port:
	push ebp
	mov ebp, esp
	push edx

	mov edx, [ebp + 8]
	mov al, [ebp + 12]
	out dx, al
	nop
	nop

	pop edx
	mov esp, ebp
	pop ebp
	ret

read_from_port:
	push ebp
	mov ebp, esp
	push edx

	mov edx, [ebp + 8]	
	xor eax, eax
	in al, dx
	nop
	nop
	pop edx
	mov esp, ebp
	pop ebp
	ret	
