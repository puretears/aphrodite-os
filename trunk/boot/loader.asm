	org 0100H
	
	jmp LABEL_START

	TOP_OF_STACK		equ 0100H
	BASE_OF_KERNEL		equ 08000H
	OFFSET_OF_KERNEL	equ 0100H

	%include "fat12header.inc"

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	mov sp, 0100H

	call clear_screen
	
	push str_loading_msg
	call disp_str
	add sp, 2

	push str_kernel_name
	push OFFSET_OF_KERNEL
	push BASE_OF_KERNEL
	call load_file
	add sp, 6
	cmp ax, 0
	jz NO_KERNEL_FOUND
	jmp INIT_FOR_KERNEL
NO_KERNEL_FOUND:
	push str_loading_failed
	call disp_str
	add sp, 2
	jmp $

INIT_FOR_KERNEL:
	push db_ard_buffer
	push ds
	push dw_ard_entries
	call get_mem_mapinfo
	add sp, 6

	push db_ard_buffer
	push ds
	push word [dw_ard_entries]
	call get_mem_size
	add sp, 6

	jmp BASE_OF_KERNEL:OFFSET_OF_KERNEL
;================================ Auxiliary Functions ================================
clear_screen:
	enter 0, 0
	push gs
	push di

	mov ax, 0B800H
	mov gs, ax

	xor ax, ax
	mov al, 20H
	mov di, 0
CONTINUE_CLEAR:
	mov [gs:di], ax
	add di, 2
	cmp di, 3998
	ja CLEAR_OVER
	jmp CONTINUE_CLEAR
CLEAR_OVER:
	mov word [dw_curr_pos], 0
	pop di
	pop gs
	leave
	ret
; push [string address]
; call disp_str
disp_str:
	enter 0, 0
	push gs
	push si
	push di
	mov ax, 0B800H
	mov gs, ax
	mov si, [bp + 4]
	mov di, [dw_curr_pos]
GO_ON_DISP:
	lodsb
	cmp al, 0
	jz DISP_OVER
	mov ah, 0CH
	mov [gs:di], ax
	add di, 2
	jmp GO_ON_DISP
DISP_OVER:
	mov [dw_curr_pos], di
	pop di
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

	mov ax, BASE_OF_KERNEL
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
	add sp, 4
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
	push es
	push bx
	push cx
	push si
	push di

;Phrase 1: Load all rootdir table entries into memory.
READ_SECTOR:
	; Load all rootdir sectors into memory
	mov bx, [bp + 6] ; offset of buffer
	mov ax, [bp + 4]
	mov es, ax ; es:bx buffer of the file
	push word [dw_sector_of_rootdir]
	push 14
	call read_sector
	add sp, 4

;Phrase 2: Find the corresponding "loader.bin" entry
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
	and di, 0FFE0H	; 32-bytes alignment
	add di, 20H		; Next rootdir entry
	mov cx, 11
	jmp COMPARE_NAME

ALL_ROOTDIR_ENTRIES_READ:
	xor ax, ax
	jmp FAILED_RETURN

;Phrase 3: Load "loader.bin" into memroy
ENTRY_FOUND:
	and di, 0FFE0H
	add di, 1AH
	; Display a dot for loading a sector
	push str_loading_dot
	call disp_str
	add sp, 2
	mov dx, [es:di]
	mov [dw_fat_table_entry], dx
CONTINUE_LOAD:
	add dx, OFFSET_OF_DATASEC
	push dx
	push 1
	call read_sector
	add sp, 4
	
	push word [dw_fat_table_entry]
	call get_fat_entry
	add sp, 2
	cmp ax, 0FF8H
	jae LOAD_OVER
	add bx, 200H
	mov [dw_fat_table_entry], ax
	mov dx, ax
	jmp CONTINUE_LOAD
LOAD_OVER:
	mov ax, 1
FAILED_RETURN:
	pop di
	pop si
	pop cx
	pop bx
	pop es
	leave
	ret

; short get_mem_mapinfo([out]short *size, [in]short base_of_ard, [in]short offset_of_ard)
get_mem_mapinfo:
	enter 0, 0
	push ebx
	push ecx
	push edx
	push si
	push di
	push es
	mov si, [bp + 4]
	mov ax, [bp + 6]
	mov es, ax
	mov di, [bp + 8] ; es:bx - address of the buffer
GET_MEM_INFO:
	mov di, db_ard_buffer
	mov ebx, 0
	mov edx, 534D4150H ; 'SMAP' signature
CONTINUE_GET_MEMINFO:
	mov eax, 0E820H
	mov ecx, 20
	int 15H
	inc word [si]
	jc GET_MEMINFO_ERROR
	cmp eax, 534D4150H
	jnz MEMINFO_CHECKSUM_ERROR
	test ebx, ebx
	jz GET_MEMINFO_SUCCESSFUL
	add di, 20
	jmp CONTINUE_GET_MEMINFO
GET_MEMINFO_ERROR:
MEMINFO_CHECKSUM_ERROR:
	mov word [si], 0
	xor ax, ax
GET_MEMINFO_SUCCESSFUL:
	mov ax, 1
	pop es
	pop di
	pop si
	pop edx
	pop ecx
	pop ebx
	leave
	ret

; int get_mem_size([in]short ard_entries, [in]short base_of_ard, [in]short offset_of_ard)
get_mem_size:
	enter 0, 0
	push di
	push ds
	push cx
	mov di, [bp + 8]
	mov ax, [bp + 6]
	mov ds, ax
	mov cx, [bp + 4]
	xor eax, eax

CONTINUE_CALC_MEMSIZE:
	add eax, dword [di + 8]
	add di, 20
	loop CONTINUE_CALC_MEMSIZE
	pop cx
	pop ds
	pop edi
	leave
	ret

str_loading_dot			db ".", 0
str_loading_failed		db "No kernel.bin found", 0
str_loading_msg			db "Loading kernel", 0
str_kernel_name			db "KERNEL  BIN", 0
db_odd_index			db 0
db_ard_buffer times 512 db 0 ; 25 ard entries support
dw_sector_of_rootdir	dw SECTOR_OF_ROOTDIR
dw_total_rootdir_sec	dw TOTAL_ROOTDIR_SEC
dw_direntry_per_sec		dw DIRENTRY_PER_SEC
dw_total_rootdir_entry	dw TOTAL_ROOTDIR_ENTRY
dw_curr_pos				dw 0
dw_fat_table_entry		dw 0
dw_ard_entries			dw 0
dd_total_mem_size		dd 0
