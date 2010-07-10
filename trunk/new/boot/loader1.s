global loader
extern text, end, bss, loader, __cxa_finalize

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
	; Possibly set up a stack here: mov esp, stack + STACKSIZE
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

	mov [swapper_pg_dir], pg0 + 7
	mov [swapper_pg_dir + 3072], pg0 + 7

	; Initialize pg0
	mov edi, pg0 + 4092
	mov eax, 0x003FF007
	mov ecx, 1024
	std 
1:
	stdsd
	sub eax, 01000H
	jae 1

	mov eax, swapper_pg_dir
	mov cr3, eax
	mov eax, cr0
	or cr0, 080000000H
	mov eax, cr0
	jmp 2
2:
	leave
	ret

	times (1000 - $) db 0
swapper_pg_dir:
	times (2000 - $) db 0
pg0:

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
	
	dw 0	; Make pesudo_gdt aligned 4 byte boundary.
gdt_pesudo:
	dw $ - gdt_kernel
	dd KERNEL_BASE + gdt_kernel
	
section .bss
align 4
stack:
	resb STACKSIZE
