global cur_pos

BOOT_ADDR	equ 09000H
LOADER_ADDR	equ 09020H
KERNEL_ADDR equ 01000H

	mov ax, BOOT_ADDR
	mov ds, ax
	mov ax, LOADER_ADDR
	mov es, ax

	mov ah, 03H
	xor bh, bh
	int 10H
	mov [0], dx			; Save current cursor position at 0x90000

	mov ah, 0FH
	int 10H
	mov [12], bx		; bh = current page
	mov [14], ax		; ah = window width, al = video mode

	mov ah, 12H
	mov bl, 10H
	int 10H
	mov [16], ax
	mov [18], bx		; bh = display mode, bl = display memory
	mov [20], cx		; cx = video card parameter

	; Load the 1st disk parameter into 0x90080
	mov ax, 0
	mov ds, ax
	lds si, [41 * 4]
	mov ax, BOOT_ADDR
	mov es, ax
	mov di, 080H
	mov cx, 10H
	rep movsb
	
	; Get memory map
	mov ax, cs
	mov ds, ax
	mov es, ax
	call get_mem_map

	cli
	cld
	; Move system module from 0x10000 to 0x00000
	xor ax, ax
CONTINUE_COPY:
	mov es, ax
	add ax, 01000H
	cmp ax, 09000H
	jz COPY_OVER
	mov ds, ax
	xor si, si
	xor di, di
	mov cx, 08000H
	rep movsw
	jmp CONTINUE_COPY
COPY_OVER:

	mov ax, cs
	mov ds, ax
	mov si, loader
	call disp_str
	mov si, str_kernel_base
	call disp_str
	mov si, mem_map_hint
	call disp_str
	mov si, mem_map_title
	call disp_str
	call disp_mem_map
	call calc_phy_mem_size
	push eax
	mov si, str_total_mem
	call disp_str
	pop eax
	push eax
	call disp_int
	add sp, 4
	call enableA20
	call init8259A

	lgdt [gdt_pesudo]
	;lidt [idt_pesudo]
	; Jump to protect mode
	mov eax, cr0
	or eax, 1
	mov cr0, eax
	jmp dword CODE32_SEL:0


enableA20:
	in al, 92H
	or al, 00000010B
	out 92H, al
	ret

init8259A:
	; Init ICW1
	mov al, 011H
	out 20H, al
	out 0A0H, al
	; Init ICW2
	mov al, 020H
	out 21H, al
	mov al, 28H
	out 0A1H, al
	; Init ICW3
	mov al, 4H
	out 21H, al
	mov al, 2H
	out 0A1H, al
	; Init ICW4
	mov al, 1
	out 21H, al
	out 0A1H, al
	; Prohibit all interrupts
	mov al, 0FFH
	out 21H, al
	out 0A1H, al
	ret

get_mem_map:
	enter 0, 0
	push ebx
	push ecx
	push edx
	push edi

	mov eax, 0E820H
	mov ebx, 0
	mov ecx, 512
	mov edx, 0534D4150H
	mov edi, _mem_map_buffer
INT15H:
	int 15H
	jnc CONTINUE_TO_SCAN
	jmp $		; If error, just hang here just for simplicity.
CONTINUE_TO_SCAN:
	inc word [_mem_map_count]
	cmp ebx, 0
	jz SCAN_END
	add edi, ecx
	mov eax, 0E820H
	jmp INT15H
SCAN_END:

	pop edi
	pop edx
	pop ecx
	pop ebx
	leave
	ret

disp_mem_map:
	enter 0, 0
	push ds
	push esi
	push bx
	push cx
	push dx
	mov cx, word [_mem_map_count]
	xor bx, bx
DISP_MEM_MAP_BUF:
	mov dx, 5
DISP_MEM_BUF_ENTRY:	
	lea esi, [_mem_map_buffer + bx]	
	lodsd
	push eax
	call disp_int
	add sp, 4
	add bx, 4
	mov al, 'H'
	call disp_al
	mov al, 32
	call disp_al
	dec dx
	cmp dx, 0
	jnz DISP_MEM_BUF_ENTRY
	call disp_newline
	loop DISP_MEM_MAP_BUF
	pop dx
	pop cx
	pop bx
	pop esi
	pop ds
	leave
	ret

calc_phy_mem_size:
	enter 0, 0
	push ebx
	push edx
	push si
	push di
	xor dx, dx
	mov ax, word [_mem_map_count]
CONTINUE_FIND_LVE:
	dec ax
	push ax
	mov bx, 20
	mul bx	; We get the last mem_map entry offset here
	add ax, _mem_map_buffer
	mov di, ax
	add ax, 16
	mov si, ax
	lodsd
	cmp eax, 1
	jz GET_LAST_VALID_ENTRY
	xor dx, dx
	pop ax
	jmp CONTINUE_FIND_LVE
GET_LAST_VALID_ENTRY:
	xchg di, si
	lodsd	; eax = low base addr 
	mov ebx, eax
	add si, 4
	lodsd	; eax = low limit
	add eax, ebx	; eax = total physical memory
	xor edx, edx
	mov ebx, 1024 * 1024
	div ebx	; eax = Physical memory in MB
	pop di
	pop si
	pop edx
	pop ebx
	leave
	ret

%include "display.inc"
%include "protect.inc"
loader		db "Load loader to 0x90200.", 0AH, 0
str_kernel_base db "Load kernel to 0x00000.", 0AH, 0
mem_map_title db "BaseAddrL BaseAddrH LimitL    LimitH    Type", 0AH, 0
mem_map_hint db "The memory map: ", 0AH, 0
str_total_mem db "The memory size: ", 0
curr_pos	dw 160
_mem_map_buffer times 512 db 0
_mem_map_count dw 0
mem_size_low dd 0
mem_size_high dd 0

; Temporary GDT
gdt: gdt_desc 0, 0, 0
code: gdt_desc 0FFFFH, 0, 0C09AH	; Code segment
data: gdt_desc 0FFFFH, 0, 0C092H 

CODE32_SEL equ code - gdt

gdt_pesudo dw $ - gdt
		   dd 090200H + gdt

