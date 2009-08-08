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
TOTAL_ROOTDIR_ENTRY equ 224

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

	; Reset floopy disk
	xor ax, ax
	xor dx, dx
	int 13H

TRY_TO_RELOAD:
	push str_loader_name
	push OFFSET_OF_LOADER
	push BASE_OF_LOADER
	call load_file
	cmp ax, 1
	jnz LOAD_LOADER_FAILED
	; Jump to the loader
	mov di, 0
	push str_boot_msg
	call disp_str
	;jmp BASE_OF_LOADER:OFFSET_OF_LOADER
	jmp $
LOAD_LOADER_FAILED:
	jmp TRY_TO_RELOAD ; Try to reload the loader until successful

;================================ Auxiliary Functions ================================
; push [string address]
; call disp_str
disp_str:
	enter 0, 0
	push gs
	push si
	mov ax, 0B800H
	mov gs, ax
	mov si, [bp + 4]
GO_ON_DISP:
	lodsb
	cmp al, 0
	jz DISP_OVER
	mov ah, 0CH
	mov [gs:di], ax
	add di, 2
	jmp GO_ON_DISP
DISP_OVER:
	pop si
	pop gs
	leave
	ret

; es:bx data buffer
; push starting sector number
; push numbers of sectors want to read
; call read_sector
read_sector:
	enter 6, 0
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
	enter 0,0
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
	enter 0, 0
	push bx
READ_SECTOR:
	; Load all rootdir sectors into memory
	mov bx, [bp + 6] ; offset of buffer
	mov ax, [bp + 4]
	mov es, ax ; es:bx buffer of the file
	push word [dw_sector_of_rootdir]
	push 14
	call read_sector

	mov di, bx
	mov cx, 11
COMPARE_NAME:
	mov si, [bp + 8]
CONTINUE_MATCH:
	lodsb
	cmp al, [es:di]
	jnz	NEXT_ENTRY
	inc di
	loop CONTINUE_MATCH
	jmp ENTRY_FOUND
NEXT_ENTRY:
	dec word [dw_total_rootdir_entry]
	jz ALL_ROOTDIR_ENTRIES_READ
	and di, 0FFE0H ; 32-bytes alignment
	add di, 20H	; Next rootdir entry
	mov cx, 11
	jmp COMPARE_NAME

ALL_ROOTDIR_ENTRIES_READ:
	xor ax, ax
ENTRY_FOUND:
	mov ax, 1
	pop bx
	leave
	ret

str_boot_msg			db "Booting...", 0
str_loader_name			db "LOADER  BIN", 0
db_odd_index			db 0
dw_sector_of_rootdir	dw SECTOR_OF_ROOTDIR
dw_total_rootdir_sec	dw TOTAL_ROOTDIR_SEC
dw_direntry_per_sec		dw DIRENTRY_PER_SEC
dw_total_rootdir_entry	dw TOTAL_ROOTDIR_ENTRY

	times (510 - ($ - $$)) db 0
	dw 0AA55H
