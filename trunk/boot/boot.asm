%ifdef DEBUG
	org 0100H
BASE_OF_STACK	equ 0100H
%else
	org 07C00H
BASE_OF_STACK	equ 07C00H
%endif

BASE_OF_LOADER		equ 9000H
OFFSET_OF_LOADER	equ 0100H
SECTOR_OF_ROOTDIR	equ 19
BYTES_PER_SECTOR	equ 200H
SECTORS_PER_TRACK	equ 18
TOTAL_ROOTDIR_SEC	equ 14
DIRENTRY_PER_SEC	equ 16

	jmp short LABEL_START
	nop

	;BPB here
	BS_OEMName			db "PURETEAR"
	BPB_BytesPerSec		dw 0200H
	BPB_SecPerClus		db 1
	BPB_RsvdSecCnt		dw 1
	BPB_NumFATs			db 2
	BPB_RootEntCnt		dw 0E0H
	BPB_TotSec16		dw 0B40H
	BPB_Media			db 0xF0
	BPB_FATSz16			dw 9
	BPB_SecPerTrk		dw 12H
	BPB_NumHeads		dw 2
	BPB_HiddenSec		dd 0
	BPB_TotSec32		dd 0
	BS_DrvNum			db 0
	BS_Reserved1		db 0
	BS_BootSig			db 0x29
	BS_VolID			dd 0
	BS_VolLab			db "puretears  "
	BS_FileSysType		db "FAT12   "

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BASE_OF_STACK

	; Reset floppy disk
	xor ah, ah
	xor dl, dl
	int 13H

	mov ax, BASE_OF_LOADER
	mov es, ax

READ_SECTOR:
	cmp word [dw_total_rootdir_sec], 0 ; Root directory occupies 14 sectors at most
	jz ALL_SECTORS_READ
	mov bx, OFFSET_OF_LOADER
	push word [dw_sector_of_rootdir]
	push 1
	call read_sector

ROOTDIR_ENTRY:
	mov di, OFFSET_OF_LOADER
NEXT_ROOTDIR_ENTRY:
	mov si, str_loader_name
	mov cx, 11
COMPARE_NAME:
	lodsb
	cmp byte [es:di], al
	jz COMPARE_NEXT_CHAR
	cmp word [dw_direntry_per_sec], 0
	jz READ_NEXT_SECTOR
	dec word [dw_direntry_per_sec]
	and di, 0FFE0H
	add di, 20H
	jmp NEXT_ROOTDIR_ENTRY

COMPARE_NEXT_CHAR:
	inc di
	dec cx
	jcxz FOUND_LOADER
	jmp COMPARE_NAME

READ_NEXT_SECTOR:
	mov word [dw_direntry_per_sec], DIRENTRY_PER_SEC
	dec word [dw_total_rootdir_sec]
	inc word [dw_sector_of_rootdir]
	jmp READ_SECTOR

ALL_SECTORS_READ:
	mov ax, 1234H
	jmp $

FOUND_LOADER:
	mov ax, 9876H
	jmp BASE_OF_LOADER:OFFSET_OF_LOADER

; es:bx data buffer
; push starting sector number
; push numbers of sectors want to read
; call read_sector
read_sector:
	enter 6
	push cx
	push dx

	xor dx, dx
	mov ax, [bp + 6] ; starting sector number
	mov cx, SECTORS_PER_TRACK
	div cx

	inc dx
	mov [bp - 2], dx ; sector number

	mov cx, ax
	shr ax, 1
	mov [bp - 4], ax ; track number

	and cx, 1
	mov [bp - 6], cx ; head number
	
	mov ch, [bp - 4]
	mov cl, [bp - 2]
	mov dh, [bp - 6]
	mov dl, 0
	
CONTINUE_READING:
	mov ah, 02H
	mov al, [bp + 4]
	int 13H
	jc CONTINUE_READING ; Continue reading until successful
	
	pop dx
	pop cx
	add sp, 6
	leave
	ret

; push entry_index
; call get_fat_entry
get_fat_entry:
	enter
	push es
	push bx
	push dx

	mov ax, BASE_OF_LOADER
	sub ax, 0100H
	mov es, ax
	
	mov ax, [bp + 4] ; entry index of fat table
	; Whether the index is an odd
	xor dx, dx
	mov bx, 3
	mul bx
	mov bx, 2
	div bx
	cmp dx, 0
	jz EVEN_INDEX
	mov byte [db_odd_index], 1
EVEN_INDEX:
	xor dx, dx
	mov bx, [BPB_BytesPerSec]
	div bx
	xor bx, bx
	add ax, 1
	push ax
	push 2
	call read_sector
	add bx, dx
	mov ax, [es:bx]
	cmp byte [db_odd_index], 1
	jnz GET_ENTRY
	shr ax, 4
GET_ENTRY:
	and ax, 0FFFH
	pop dx
	pop bx
	pop es
	leave
	ret

; push [address of file name]
; push offset of buffer
; push base of buffer
; call load_file
load_file:
	enter
	
	mov bx, [bp + 6] ; offset of buffer 
	push word [dw_sector_of_rootdir]
	push 1
	call read_sector

	leave
	ret


str_loader_name			db "LOADER  BIN", 0
db_odd_index			db 0
dw_sector_of_rootdir	dw SECTOR_OF_ROOTDIR
dw_total_rootdir_sec	dw TOTAL_ROOTDIR_SEC
dw_direntry_per_sec		dw DIRENTRY_PER_SEC
	
	times (510 - ($ - $$)) db 0
	dw 0AA55H
