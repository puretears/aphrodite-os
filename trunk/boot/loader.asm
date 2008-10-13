	org 200H

BASE_OF_STACK	equ 0100H ; Because stack will come from 0100H, so we cannot
			  ; begin at 0. Above or equal 0100H is OK.

	jmp LABEL_START

%include "fat12hdr.inc"
%include "load.inc"
%include "protect.inc"

LABEL_GDT:
GDT_DUMMY:	descriptor 0, 0, 0
GDT_FLAT_C:	descriptor 0, 0FFFFFH, EXECUTE_READ | S_FLAG | P_FLAG | DB_FLAG | G_FLAG
GDT_FLAT_D:	descriptor 0, 0FFFFFH, READ_WR | S_FLAG | P_FLAG | DB_FLAG | G_FLAG
GDT_VEDIO:	descriptor 0B8000H, 0FFFFH, READ_WR | S_FLAG | P_FLAG | DPL3
GDT_LEN	equ $ - GDT_DUMMY

FLAT_C_SEL	equ GDT_FLAT_C - GDT_DUMMY
FLAT_D_SEL	equ GDT_FLAT_D - GDT_DUMMY
VEDIO_SEL	equ GDT_VEDIO - GDT_DUMMY + 3

PESUDO_GDT:	dw GDT_LEN - 1
		dd LOADERBASE_PHY_ADDR + LABEL_GDT 

LABEL_START:
	mov ax, cs
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, BASE_OF_STACK

	; Check memory first
	mov ebx, 0
	mov di, __mem_chk_buff
	; Scan memory
LABEL_MEM_CHK:
	mov eax, 0E820H
	mov ecx, 20
	mov edx, 534D4150H
	int 15H
	jc LABEL_MEM_CHK_FAIL
	add di, 20
	inc dword [__ard_count]
	cmp ebx, 0
	jz LABEL_MEM_CHK_SUCCESS
	jmp LABEL_MEM_CHK

LABEL_MEM_CHK_FAIL:
	mov ax, 0B800H
	mov es, ax
	mov di, 0
	mov si, __str_mem_chk_fail
	call disp_str_16
	jmp $

LABEL_MEM_CHK_SUCCESS:			
	; Find kernel.bin at root directory and load kernel
	; into KERENEL_BASE:KERNEL_OFFSET
	xor ah, ah
	xor dl, dl
	int 13H
	mov ax, LABEL_GDT
	mov ax, KERNEL_BASE
	mov es, ax
	mov ax, 19

READ_NEXT_SECTOR2:
	cmp word [rootdir_sector], 0
	jz LABEL_NO_KERNEL
	dec word [rootdir_sector]

	push ax
	; read from root directory
	mov bx, KERNEL_OFFSET
	mov cl, 1
	call read_sector

	mov cx, 16
	mov di, KERNEL_OFFSET

LABEL_FIND_NEXT_ENTRY2:
	mov bx, 11
	mov si, str_kernel_name
CONTINUE_MATCHING:
	lodsb
	cmp al, [es:di]
	jnz LABEL_FIND_NEXT_ENTRY1
	dec bx
	cmp bx, 0
	jz KERNEL_FOUND
	inc di
	jmp CONTINUE_MATCHING

LABEL_FIND_NEXT_ENTRY1:
	dec cx
	jcxz READ_NEXT_SECTOR1
	and di, 0FFE0H
	add di, 32
	jmp LABEL_FIND_NEXT_ENTRY2
READ_NEXT_SECTOR1:
	pop ax
	inc ax
	jmp READ_NEXT_SECTOR2

LABEL_NO_KERNEL:
	;call clear_scr
	; Due to bochs bug, we use the following snippet to clear screen
	mov ax, 0B800H
	mov es, ax
	xor di, di
	mov ah, 05H
	mov al, ' '
	mov cx, 80 * 25
	rep stosw
	
	mov di, 0
	mov ax, cs
	mov ds, ax
	mov si, str_no_kernel
	call disp_str_16
	jmp $	; No kenrel file, hUng here!

KERNEL_FOUND:
	and di, 0FFE0H
	add di, 26	; offset of beginning sector
	mov ax, [es:di]

	mov bx, KERNEL_OFFSET

	push es
	push ax
	push si
	mov ax, 0B800H
	mov es, ax
	mov si, str_kernel_load
	mov di, 160
	call disp_str_16
	pop si
	pop ax
	pop es

GO_ON_LOADING:
	push es
	push ax
	mov ax, 0B800H
	mov es, ax
	mov ah, 08H
	mov al, '.'
	mov [es:di], ax
	add di, 2
	pop ax
	pop es

	push ax		; save this beginning sector
	add ax, 14
	add ax, 17
	mov cl, 1
	call read_sector
	
	pop ax
	call get_fat_entry
	cmp ax, 0FFFH
	jz FINISH_LOADING
	add bx, 200H
	jmp GO_ON_LOADING

FINISH_LOADING:
	call kill_motor

	; After load kernel into memory, we'll get ready to set up protect mode!
	lgdt [PESUDO_GDT]
	cli
	; Open A20 address
	in al, 92H
	or al, 00000010B
	out 92H, al
	; Enable protect mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	
	mov eax, LABEL_PM_START
	jmp dword FLAT_C_SEL:(LOADERBASE_PHY_ADDR + LABEL_PM_START)

str_loading	db "Loading", 0
str_kernel_name	db "KERNEL  BIN", 0
str_no_kernel	db "No kernel was found!", 0
bOdd		db 0
str_kernel_load	db "Loading kernel", 0
rootdir_sector	dw 14

	

; es:bx: points to buffer
; cl: number of sectors to read
; ax: starting sector number	
read_sector:
	push bp
	mov bp, sp
	sub sp, 8
	push cx
	push dx
	push bx

	xor dx, dx
	mov bx, 18
	div bx
	mov [bp - 2], ax	; track
	mov [bp - 4], ax	; head
	mov [bp - 6], dx	; sector

	shr byte [bp - 2], 1	; cylinder
	and byte [bp - 4], 1	; head
	add byte [bp - 6], 1	; sector
	
	pop bx
	mov ah, 02H
	mov al, cl
	mov ch, [bp - 2]
	mov cl, [bp - 6]
	mov dh, [bp - 4]
	mov dl, 0
	int 13H
	
	pop dx
	pop cx

	mov sp, bp
	pop bp
	ret

; ax: sector number		
get_fat_entry:
	push bp
	mov bp, sp
	push dx
	push bx
	push cx
	push es

	
	mov dx, KERNEL_BASE
	sub dx, 1000H
	mov es, dx

	xor dx, dx
	mov bx, 3
	mul bx
	mov bx, 2	; ax * 3 / 2
	div bx

	cmp dx, 0
	jz LABEL_EVEN
	mov byte [bOdd], 1

LABEL_EVEN:	
	xor dx, dx
	mov bx, 512
	div bx
	push dx		; save the remainder

	add ax, 1	; 1 beginning sector of fat table
	xor bx, bx
	mov cl, 2
	call read_sector

	pop dx
	add bx, dx
	mov ax, [es:bx]
	cmp byte [bOdd], 1
	jnz .1
	shr ax, 4
.1:
	and ax, 0FFFH
LABEL_RET:
	mov byte [bOdd], 0 ; Restore the bOdd byte!!!!!
	pop es
	pop cx
	pop bx
	pop dx
	mov sp, bp
	pop bp
	ret

; Close motor
kill_motor:
	push dx
	mov dx, 03F2H
	mov al, 0
	out dx, al
	pop dx
	ret

;--------------------------------------------------
; Display a string pointed by ds:si
disp_str_16:
	push bp
	mov bp, sp
	push ax
	push si

	mov ah, 08H

GO_ON_DISP16:
	lodsb
	cmp al, 0
	je DISP_OVER16
	cmp al, 0AH
	jne GENERAL_CHAR16
	
	call disp_ret_16
	jmp RET_CHAR16

GENERAL_CHAR16:
	stosw
RET_CHAR16:
	jmp GO_ON_DISP16

DISP_OVER16:
	
	pop si
	pop ax
	mov sp, bp
	pop bp
	ret

;--------------------------------------------------
; Display an Enter
disp_ret_16:
	push bp
	mov bp, sp
	push ax
	push bx
	push dx

	xor dx, dx
	mov ax, di
	mov bx, 160
	div bx
	inc ax
	xor dx, dx
	mul bx
	mov di, ax

	pop dx
	pop bx
	pop ax

	mov sp, bp
	pop bp
	ret

[section .s32]
align 32
[bits 32]
LABEL_PM_START:
	mov ax, VEDIO_SEL
	mov es, ax
	mov ax, FLAT_D_SEL
	mov ds, ax
	mov gs, ax
	mov fs, ax
	mov ss, ax
	mov esp, TOPOFSTACK

	mov edi, 320
	mov esi, str_mem_chk_title
	call disp_str
	call disp_mem_info
	call setup_paging
	call init_kernel
	mov ax, VEDIO_SEL
	mov gs, ax
	mov ax, FLAT_D_SEL
	mov es, ax
	jmp FLAT_C_SEL:KERNEL_ENTRYPOINT_PHY_ADDR
	jmp $

disp_mem_info:
	push ebp
	mov ebp, esp
	pushad
	mov esi, mem_chk_buff
	mov edi, 480
	mov ebx, [ard_count]

NEXT_ARD_ENTRY:
	cmp ebx, 0
	jz ARD_DISP_OVER
	mov edx, ard
	dec ebx
	mov ecx, 5
GO_ON_DISP_ARD_ENTRY:
	lodsd
	call disp_int
	mov [edx], eax
	add edx, 4
	mov ah, 05H
	mov al, 'H'
	stosw
	mov ax, 0520H
	stosw
	loop GO_ON_DISP_ARD_ENTRY
	call disp_ret
	cmp dword [type], 1
	jnz MEM_ADDR_UNAVAIL
	mov eax, [base_addr_low]
	add eax, [limit_low]
	cmp eax, [mem_size]
	jbe MEM_ADDR_UNAVAIL
	mov [mem_size], eax
MEM_ADDR_UNAVAIL:
	jmp NEXT_ARD_ENTRY
ARD_DISP_OVER:
	mov esi, str_mem_size 
	call disp_str
	mov eax, [mem_size]
	call disp_int
	mov [curr_disp_pos], edi
	popad
	mov esp, ebp
	pop ebp
	ret

setup_paging:
	push ebp
	mov ebp, esp
	pushad
	push es

	xor edx, edx
	mov eax, [mem_size]
	mov ebx, 400000H
	div ebx
	mov ecx, eax
	test eax, eax
	jz NO_REMAINDER
	inc ecx
	push ecx

NO_REMAINDER:
	; Setup page directory first
	mov ax, FLAT_D_SEL
	mov es, ax
	mov edi, PAGE_DIR_BASE
	xor eax, eax
	mov eax, PAGE_TBL_BASE | PRESENT_FLAG | RW_FLAG | US_FLAG
INIT_PDE:
	stosd
	add eax, 1000H
	loop INIT_PDE

	; Setup page table
	pop eax
	xor edx, edx
	mov ebx, 1024
	mul ebx
	mov ecx, eax
	
	mov edi, PAGE_TBL_BASE
	xor eax, eax
	mov eax, PRESENT_FLAG | RW_FLAG | US_FLAG
INIT_PTE:
	stosd
	add eax, 1000H
	loop INIT_PTE
	
	mov eax, PAGE_DIR_BASE
	mov cr3, eax
	mov eax, cr0
	or eax, 80000000H
	mov cr0, eax

	jmp .clear_cache
.clear_cache:
	nop

	pop es
	popad
	mov esp, ebp
	pop ebp
	ret

init_kernel:
	push ebp
	mov ebp, esp
	push esi
	push ecx
	push es

	mov ax, FLAT_D_SEL
	mov es, ax
	; Get virtual address of program header table
	xor esi, esi
	mov cx, [KERNELBASE_PHY_ADDR + 2CH]
	movzx ecx, cx ; ECX the number of entries in program header table
	mov esi, [KERNELBASE_PHY_ADDR + 1CH]
	add esi, KERNELBASE_PHY_ADDR ; ESI program header virtual address
LOAD_PROGRAM_HEADER:
	mov eax, [esi + 0]
	cmp eax, 0
	jz NULL_PROGRAM_HEADER
	push dword [esi + 10H]	; p_filesz	
	mov eax, [esi + 04H]
	add eax, KERNELBASE_PHY_ADDR
	push eax		; source
	push dword [esi + 08H]
	call mem_cpy
	add esp, 12
NULL_PROGRAM_HEADER:
	add esi, 20H
	loop LOAD_PROGRAM_HEADER

	pop es
	pop ecx
	pop esi
	mov esp, ebp
	pop ebp
	ret	

	%include "lib32.inc"

[section .stack32]
align 32
LABEL_PM_STACK:
	times 1024 db 0
TOPOFSTACK equ LOADERBASE_PHY_ADDR + $

[section .data32]
align 32
LABEL_DATA32:
; Variables used in real mode
__mem_chk_buff:	times 256 db 0
__str_mem_chk_fail 	  db "Memroy check failed!", 0
__str_mem_chk_title 	  db "BaseAddrL BaseAddrH LengthLow LengthHigh Type", 0AH, 0
__str_mem_size		  db "Total memory size is: ", 0
__curr_disp_pos		  dd 0
__ard_count		  dd 0
__mem_size		  dd 0
__ard:
__base_addr_low		dd 0
__base_addr_high	dd 0
__limit_low		dd 0
__limit_high		dd 0
__type			dd 0

; Variables used in protect mode
mem_chk_buff	  equ LOADERBASE_PHY_ADDR + __mem_chk_buff
ard_count	  equ LOADERBASE_PHY_ADDR + __ard_count
str_mem_chk_title equ LOADERBASE_PHY_ADDR + __str_mem_chk_title
str_mem_size	  equ LOADERBASE_PHY_ADDR + __str_mem_size
curr_disp_pos	  equ LOADERBASE_PHY_ADDR + __curr_disp_pos
mem_size	  equ LOADERBASE_PHY_ADDR + __mem_size

ard		  equ LOADERBASE_PHY_ADDR + __ard
base_addr_low	  equ LOADERBASE_PHY_ADDR + __base_addr_low
base_addr_high	  equ LOADERBASE_PHY_ADDR + __base_addr_high
limit_low	  equ LOADERBASE_PHY_ADDR + __limit_low
limit_high	  equ LOADERBASE_PHY_ADDR + __limit_high
type		  equ LOADERBASE_PHY_ADDR + __type
