global loader
global swapper_pg_dir
extern text, end, bss, __cxa_finalize, kmain

MODULEALIGN equ	1<<0                    ; align loaded modules on page boundaries
MEMINFO     equ	1<<1                    ; provide memory map
FLAGS       equ MODULEALIGN | MEMINFO	; this is the Multiboot 'flag' field
MAGIC       equ 0x1BADB002				; 'magic number' lets bootloader find the header
KERNEL_BASE equ 0C0000000H
CHECKSUM    equ -(MAGIC + FLAGS)        ; checksum required
NR_TASKS	equ 128

section .text
align 4
MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
	dd MultiBootHeader
	; The follow variables are defined in linker.ld
	dd text
	dd end
	dd bss
	dd loader
STACKSIZE equ 0x4000

loader:
	mov esp, stack
	push eax
	push ebx
	call setup_paging
	lgdt [gdt_pesudo]
	jmp KERNEL_CODE_SEL:REFRESH_CACHE

REFRESH_CACHE:
	mov ax, KERNEL_DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
;	mov esp, stack

	; Possibly set up a stack here: mov esp, stack + STACKSIZE
	pop ebx
	pop eax
	push eax                        ; Multiboot magic number
	push ebx                        ; Multiboot info structure
	call kmain

	push 0
	call __cxa_finalize;
	add esp, 4
	hlt
	
setup_paging:
	enter 0, 0
	mov ecx, 2048; ; swapper_pg_dir and pg0
	xor eax, eax
	mov edi, swapper_pg_dir
	cld
	rep stosd

	mov dword [swapper_pg_dir], pg0 + 7
	mov dword [swapper_pg_dir + 3072], pg0 + 7

	; Initialize pg0
	mov edi, pg0 + 4092
	mov eax, 0x003FF007
	std 
INIT_PG0:
	stosd
	sub eax, 01000H
	jae INIT_PG0

	; Init cr3
	mov eax, swapper_pg_dir
	mov cr3, eax

	; Enable paging
	mov eax, cr0
	or eax, 080000000H
	mov cr0, eax

	jmp AFTER_PAGING
AFTER_PAGING:
	leave
	ret

	times (1000H - ($ - $$)) db 0
swapper_pg_dir:
	times (2000H - ($ - $$)) db 0
pg0:
	times (3000H - ($ - $$)) db 0

align 8
gdt_kernel:
	dummy   dq 0000000000000000H
	; base = 0xC0000000, limit = 0x3FFFF, execute / read r0 code
	code_r0 dq 0C0C39A000000FFFFH
	; base = 0xC0000000, limit = 0x3FFFF, read / write r0 data
	data_r0 dq 0C0C392000000FFFFH
	; base = 0x00000000, limit = 0xBFFFF, execute / read r3 code
	code_r3 dq 00CBFA000000FFFFH
	; base = 0x00000000, limit = 0xBFFFF, read /write r3 data
	data_r3 dq 00CBF2000000FFFFH
		times NR_TASKS * 2 dq 0
KERNEL_CODE_SEL equ code_r0 - gdt_kernel
KERNEL_DATA_SEL equ data_r0 - gdt_kernel
USER_CODE_SEL   equ code_r3 - gdt_kernel
USER_DATA_SEL   equ data_r3 - gdt_kernel

	dw 0	; Make pesudo_gdt aligned 4 byte boundary.
gdt_pesudo:
	dw $ - gdt_kernel
	dd gdt_kernel
	
section .bss
align 4
stack:
	resb STACKSIZE
