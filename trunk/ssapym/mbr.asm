BOOT_ADDR	equ 09000H
MBR_BASE	equ 07C0H

mov ax,MBR_BASE
mov ds,ax
mov ax,BOOT_ADDR
mov es,ax
xor si,si
xor di,di
mov cx,256
rep movsw
jmp BOOT_ADDR:GO

GO:
mov ax, SETUP_ADDR  
mov es, ax
mov dx, 0 
mov ch, 0
mov cl, 2
mov ah, 2
mov al, SETUP_LEN
xor bx, bx
int 13H


times (510-($-$$)) db 0
dw 0AA55H
