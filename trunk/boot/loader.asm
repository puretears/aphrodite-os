	org 0100H
	
	jmp LABEL_START

	%include "fat12header.inc"
	%include "protect.inc"
	%include "loader.inc"

LABEL_GDT:	descriptor 0, 0, 0
CODE32:		descriptor 0, 0FFFFFH, 0C09AH 
DATA:		descriptor 0, 0FFFFFH, 0C092H 
VEDIO:		descriptor 0B8000H, 0FFFFH, 92H

PESUDO_GDT: dw ($ - LABEL_GDT - 1)
			dd (PA_LOADER + LABEL_GDT)

CODE32_SEL	equ CODE32 - LABEL_GDT
DATA_SEL	equ DATA - LABEL_GDT
VEDIO_SEL	equ VEDIO - LABEL_GDT

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	mov sp, 0100H

	call clear_screen
	;------------------------- Get memory map information ----------------------------
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
	mov dword [dd_total_mem_size], eax
	;----------------------------- Start loading kernel ---------------------------
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
	jmp ENTER_PROTECT_MODE
NO_KERNEL_FOUND:
	push str_loading_failed
	call disp_str
	add sp, 2
	jmp $

	;---------------------------- Enable protected mode -----------------------------
ENTER_PROTECT_MODE:
	call new_line
	; Load GDTR
	lgdt [PESUDO_GDT]
	; Shidld external INT
	cli
	; Open A20
	in al, 92H
	or al, 00000010B
	out 92H, al
	; CR0.PE = 1
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	; Clear instruction cache. Jump to 32-bit code
	jmp dword CODE32_SEL:(PA_LOADER + LABEL_CODE32_START)

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
	push dx
	push bx
	mov ax, 0B800H
	mov gs, ax
	xor ax, ax
	mov ah, 0CH
	mov si, [bp + 4]
	mov di, [dw_curr_pos]
GO_ON_DISP16:
	lodsb
	cmp al, 0
	jz DISP_OVER16
	cmp al, 0AH
	jnz DISP_CHAR16
	xor dx, dx
	mov ax, di
	mov bx, 160
	div bx
	inc ax
	xor dx, dx
	mul bx
	mov [dw_curr_pos], ax
	jmp DISP_OVER_RET16
DISP_CHAR16:	
	mov [gs:di], ax
	add di, 2
	jmp GO_ON_DISP16
DISP_OVER16:
	mov [dw_curr_pos], di
DISP_OVER_RET16:
	pop bx
	pop dx
	pop di
	pop si
	pop gs
	leave
	ret

;void new_line()
new_line:
	enter 0, 0
	push bx
	push dx
	xor dx, dx
	mov ax, word [dw_curr_pos]
	mov bx, 160
	div bx
	inc ax
	xor dx, dx
	mul bx
	mov word [dw_curr_pos], ax
	pop dx
	pop bx
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
	mov di, [bp + 8] ; es:di - address of the buffer
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
	pop di
	leave
	ret

[section .code32]
[bits 32]
align 32
LABEL_CODE32_START:
	mov ax, DATA_SEL
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov esp, TOP_OF_STACK32
	mov ax, VEDIO_SEL
	mov gs, ax

	push _str_mem_table_title
	call disp_str32
	add esp, 4

	call new_line32

	push _db_ard_buffer
	call disp_mem_mapinfo
	add esp, 4

	push _str_memsize_msg
	call disp_str32
	add esp, 4
	
	push dword [_dd_total_mem_size]
	call disp_int32
	add esp, 4

	push PA_PT_BASE
	push PA_PD_BASE
	push 10000000H
	call paging_init

	push _str_paging_init
	call disp_str32
	add esp, 4

	jmp $

; void paging_init(int memsize, void *pa_pd, void *pa_pt)
paging_init:
	enter 8, 0
	push ebx
	push edx
	push ecx
	push edi
	; ebp - 4: total page directories
	; ebp - 8: total page tables
	xor edx, edx
	mov eax, [ebp + 8]
	mov ebx, 400000H
	div ebx
	cmp edx, 0
	jz CALC_PT
	inc eax
CALC_PT:
	mov [ebp - 4], eax
	xor edx, edx
	mov ebx, 1024
	mul ebx
	mov [ebp - 8], eax

	; Initialize page directories
	mov eax, [ebp + 16]
	or eax, PDE_RW | PDE_US | PDE_P 
	mov edi, [ebp + 12]
	mov ecx, [ebp - 4]
CONTINUE_INIT_PDE:
	stosd
	add eax, 1000H
	loop CONTINUE_INIT_PDE

	; Initialize page tables
	xor eax, eax
	or eax, PTE_RW | PTE_US | PTE_P
	mov edi, [ebp + 16]
	mov ecx, [ebp - 8]
CONTINUE_INIT_PTE:
	stosd
	add eax, 1000H
	loop CONTINUE_INIT_PTE
	
	mov eax, cr3
	mov eax, PA_PD_BASE
	mov cr3, eax
	mov eax, cr0
	or eax, 80000000H
	mov cr0, eax
	jmp PAGING_CLEAR_INSTR_CACHE
PAGING_CLEAR_INSTR_CACHE:
	pop edi
	pop ecx
	pop edx
	pop ebx
	add esp, 8
	leave
	ret

	
;void disp_mem_mapinfo(void *buffer)	
disp_mem_mapinfo:
	enter 32, 0
	push esi
	push ebx
	push edx
	push ecx
	; ebp - 4: base address low
	; ebp - 8: base address high
	; ebp - 12: length low
	; ebp - 16: length high
	; ebp - 20: type
	; ebp - 24: alignment fill
	mov esi, [ebp + 8]
	movzx ecx, word [_dw_ard_entries]
CONTINUE_ARD_DISP:	
	lodsd
	mov [ebp - 4], eax
	lodsd
	mov [ebp - 8], eax
	lodsd
	mov [ebp - 12], eax
	lodsd
	mov [ebp - 16], eax
	add esi, 4 ; Jump type
	
	mov edx, [ebp - 4]
	add edx, [ebp - 12] ; eax = low 32-bit ending address
	mov ebx, [ebp - 8]
	adc ebx, [ebp - 16] ; ebx = high 32-bit ending address

	push dword [ebp - 8]
	call disp_int32
	add esp, 4
	push dword [ebp - 4]
	call disp_int32
	add esp, 4
	push _str_delimeter
	call disp_str32
	add esp, 4
	push ebx
	call disp_int32
	add esp, 4
	push edx
	call disp_int32
	add esp, 4

	call new_line32
	loop CONTINUE_ARD_DISP

	pop ecx
	pop edx
	pop ebx
	pop esi
	add esp, 32
	leave
	ret

; void disp_int32([in] int)
disp_int32:
	enter 0, 0
	push esi
	push edi
	push ecx
	push ebx
	mov esi, [ebp + 8]
	mov di, word [_dw_curr_pos]
	mov ah, 0CH
	mov cl, 28
GO_ON_DISP_INT32:	
	mov ebx, esi
	shr ebx, cl
	mov al, bl
	and al, 0FH
	cmp al, 0AH
	jae ADD09
	add al, 30H
	jmp DISP_AL
ADD09:
	add al, 37H
DISP_AL:
	mov [gs:di], ax
	add di, 2
	cmp cl, 0
	jz DISP_INT32_OVER
	sub cl, 4
	jmp GO_ON_DISP_INT32
DISP_INT32_OVER:
	mov word [_dw_curr_pos], di
	pop ebx
	pop ecx
	pop edi
	pop esi
	leave
	ret

; void new_line32()
new_line32:
	enter 0, 0
	push ebx
	push edx

	xor edx, edx
	movzx eax, word [_dw_curr_pos]
	mov ebx, 160
	div ebx
	xor edx, edx
	add eax, 1
	mul ebx
	mov [_dw_curr_pos], ax
	
	pop edx
	pop ebx
	leave
	ret

;void disp_str32([in]char *)
disp_str32:
	enter 0, 0
	push edi
	push esi
	push ebx
	push edx
	xor eax, eax
	mov ah, 0CH
	mov esi, [ebp + 8]
	mov di, word [_dw_curr_pos]
CONTINUE_DISP32:
	lodsb
	cmp al, 0
	jz DISP_OVER32
	cmp al, 0AH
	jnz DISP_CHAR
	; Change a new line
	mov ax, di
	xor dx, dx
	mov bx, 160
	div bx
	inc ax
	xor dx, dx
	mul bx
	mov [_dw_curr_pos], ax
	jmp DISP_OVER_RET
DISP_CHAR:
	mov [gs:di], ax
	add di, 2
	jmp CONTINUE_DISP32
DISP_OVER32:
	mov [_dw_curr_pos], di
DISP_OVER_RET:
	pop edx
	pop ebx
	pop esi
	pop edi
	leave
	ret

[section .data]
align 32
str_loading_dot			db ".", 0
str_loading_failed		db "No kernel.bin found", 0
str_loading_msg			db "Loading kernel", 0
str_kernel_name			db "KERNEL  BIN", 0
str_delimeter			db " - ", 0
str_memsize_msg			db "Total memory size: ", 0
str_paging_init			db "Paing enabled successfully", 0
db_odd_index			db 0
str_mem_table_title		db "e820: ", 0
db_ard_buffer times 512 db 0 ; 25 ard entries support
align 32
dw_sector_of_rootdir	dw SECTOR_OF_ROOTDIR
dw_total_rootdir_sec	dw TOTAL_ROOTDIR_SEC
dw_direntry_per_sec		dw DIRENTRY_PER_SEC
dw_total_rootdir_entry	dw TOTAL_ROOTDIR_ENTRY
dw_curr_pos				dw 0
dw_fat_table_entry		dw 0
dw_ard_entries			dw 0
dd_total_mem_size		dd 0

_str_mem_table_title	equ PA_LOADER + str_mem_table_title
_dw_curr_pos			equ PA_LOADER + dw_curr_pos
_db_ard_buffer			equ PA_LOADER + db_ard_buffer
_str_delimeter			equ PA_LOADER + str_delimeter
_dw_ard_entries			equ PA_LOADER + dw_ard_entries
_str_memsize_msg		equ PA_LOADER + str_memsize_msg
_dd_total_mem_size		equ PA_LOADER + dd_total_mem_size
_str_paging_init		equ PA_LOADER + str_paging_init

STACK_SPACE times 1000H db 0
TOP_OF_STACK32 equ PA_LOADER + $
