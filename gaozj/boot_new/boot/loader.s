global entry_addr,display,swapper_pg_dir
extern load_addr, load_end_addr, bss_end_addr, kmain
LOADE_ADDR equ 0100000h
NR_TASKS	equ 128
	MAGIC	equ 1BADB002H
	FLAG_ALIGNED		equ	 1<<0
	FLAG_MEMINFO	equ	1<<1
	FLAG	equ	(FLAG_ALIGNED | FLAG_MEMINFO)
	CHECKSUM	equ	-(MAGIC + FLAG)
	section .text
	align 4
	multiboot:
		dd	MAGIC
		dd	FLAG
		dd	CHECKSUM
		dd	multiboot
		dd	load_addr
		dd	load_end_addr
		dd	bss_end_addr
		dd	entry_addr
;	mov esp, stack
;STACKSIZE equ	0x4000

entry_addr:
	mov esp, stack
	push eax
	push ebx
	call steup_paging
	; 加载 GDT
	lgdt	[gdt_pesudo]
	jmp KERNEL_CODE_SEL:LABEL_PM_START 

LABEL_PM_START:		
	mov ax, KERNEL_DATA_SEL
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	
	call kmain
	hlt

steup_paging:
	enter 0, 0
	mov ecx, 2048
	xor eax, eax
	mov edi, swapper_pg_dir
	cld
	rep stosd

	mov dword [swapper_pg_dir], pg0 + 7
	mov dword [swapper_pg_dir + 3072], pg0 + 7

	mov edi, pg0 + 4092
	mov eax, 0x003FF007
	std
INIT_PG0:
	stosd
	sub eax, 01000H
	jae	INIT_PG0

	mov eax, swapper_pg_dir
	mov cr3, eax

	mov eax, cr0
	or eax, 080000000H
	mov cr0, eax

	jmp AFRER_PAGING
AFRER_PAGING:
	leave
	ret

	times (1000H - ($ -$$)) db 0

swapper_pg_dir:	times (2000H - ($ - $$)) db 0
pg0:			times (3000H - ($ - $$)) db 0

align 8
gdt_kernel:
dummy	dq	0000000000000000H
code_r0	dq	0C0C39A000000FFFFH
data_r0 dq	0C0C392000000FFFFH
code_r3	dq	00CBFA0000000FFFFH
data_r3	dq	00CBF20000000FFFFH
	times NR_TASKS *2 dq 0
KERNEL_CODE_SEL	equ code_r0 - gdt_kernel
KERNEL_DATA_SEL	equ	data_r0 - gdt_kernel
USER_CODE_SEL	equ	code_r3 - gdt_kernel + 3
USER_DATA_SEL	equ	data_r3 - gdt_kernel + 3

gdt_pesudo:
	dw $ - gdt_kernel
	dd gdt_kernel

section .bss
align	4
stack:
	resb 0x4000
