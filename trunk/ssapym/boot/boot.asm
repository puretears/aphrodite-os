BOOT_ADDR	equ 09000H
MBR_BASE	equ 07C0H
SETUP_ADDR      equ 09020H
SETUP_LEN       equ 4
KERNEL_ADDR     equ 01000H
KERNEL_SIZE     equ 03000H
KERNEL_END      equ KERNEL_ADDR + KERNEL_SIZE

;copy mbr to 0x90000
mov ax,MBR_BASE
mov ds,ax
mov ax,BOOT_ADDR
mov es,ax
xor si,si
xor di,di
mov cx,256
rep movsw
jmp BOOT_ADDR:GO

;copy 4 sector setup code to 0x90200
GO:
mov ax,SETUP_ADDR  
mov es,ax
mov dh,0		;Head
mov dl,0		;Drive 1st floppy disk
mov ch,0		;Track
mov cl,2		;Sector
mov ah,2		;AH = 02h Read Sectors From Drive
mov al,SETUP_LEN	;Sectors To Read Count
xor bx,bx		;ES:BX Buffer Address Pointer
int 13H
jc GO

;copy kernel to 0x10000
GET_SECT_PRE_TRAC:
xor dl,dl
mov ah,08H
int 13H
jc GET_SECT_PRE_TRAC
and cx,63
mov byte [secs_per_trac],cl



secs_per_trac db 0
times (510-($-$$)) db 0
dw 0AA55H
