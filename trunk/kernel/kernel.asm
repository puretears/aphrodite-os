	mov ax, cs
	mov ds, ax
	mov ss, ax
	mov sp, 04FFFH
	mov si, kernel_load
	call disp_str
	jmp $
%include "../display.inc"
kernel_load db "Load kernel at 0x10000.", 0AH, 0
curr_pos dw 320
