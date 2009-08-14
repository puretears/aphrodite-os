	org 0100H

	mov ax, 0B800H
	mov gs, ax
	mov di, ((80 * 9 + 10) * 2)
	mov ah, 0CH
	mov al, 'K'
	mov [gs:di], ax

	jmp $
