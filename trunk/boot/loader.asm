	org 0100H
	mov ax, 0B800H
	mov gs, ax
	mov ah, 0FH
	mov al, 'L'
	mov [gs:((80 * 10 + 39) * 2)], ax

	jmp $
