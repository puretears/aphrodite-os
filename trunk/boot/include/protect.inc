
; ====================  Constant  ======================
; Type
; ---------------------- S = 0 -------------------------
; 0 - 7 data segment
READ_ONLY				equ 0000H
READ_ONLY_ACCESSED			equ 0001H
READ_WR					equ 0002H
READ_WR_ACCESSED			equ 0003H
READ_ONLY_EXPAND_DOWN			equ 0004H
READ_ONLY_EXPAND_DOWN_ACCESSED		equ 0005H
READ_WR_EXPAND_DOWN			equ 0006H
READ_WR_EXPAND_DOWN_ACCESSED		equ 0007H
; 8 - 15 code segment
EXECUTE_ONLY				equ 0008H
EXECUTE_ONLY_ACCESSED			equ 0009H
EXECUTE_READ				equ 000AH
EXECUTE_READ_ACCESSED			equ 000BH
EXECUTE_ONLY_CONFORMING			equ 000CH
EXECUTE_ONLY_CONFORMING_ACCESSED	equ 000DH
EXECUTE_READ_ONLY_CONFORMING		equ 000EH
EXECUTE_READ_ONLY_CONFORMING_ACCESSED	equ 000FH
; -------------------- S = 0 END -----------------------

; ---------------------- S = 1 -------------------------
RESERVED	equ 0000H
TSS16_AVAIL	equ 0001H
LDT		equ 0002H
TSS16_BUSY	equ 0003H
CALLGATE16	equ 0004H
TASKGATE	equ 0005H
INTGATE16	equ 0006H
TRAPGATE16	equ 0007H
RESERVED1	equ 0008H
TSS32_AVAIL	equ 0009H
RESERVED2	equ 000AH
TSS32_BUSY	equ 000BH
CALLGATE32	equ 000CH
RESERVED3	equ 000DH
INTGATE32	equ 000EH
TRAPGATE32	equ 000FH
; -------------------- S = 1 END -----------------------

; S flag (s = 1 data/code segment; s = 0 system segment)
S_FLAG	equ 10H

; DPL
DPL0	equ 0000H
DPL1	equ 0020H
DPL2	equ 0040H
DPL3	equ 0060H

; P flag
P_FLAG	equ 0080H

; AVL
AVL_FLAG	equ 1000H

; L flag
L_FLAG	equ 2000H

; D/B flag
DB_FLAG	equ 4000H

; G flag
G_FLAG	equ 8000H

; ----------------------- Paging ----------------------/
PRESENT_FLAG	equ 001H
RW_FLAG		equ 002H
US_FLAG		equ 004H
PWT_FLAG	equ 008H
PCD_FLAG	equ 010H
ACCESSED_FLAG	equ 020H
DIRTY_FLAG	equ 040H	; PTE only
PS_FLAG		equ 080H	; PDE only
PAT_FLAG	equ 080H	; PTE only
GLOBAL_FLAG	equ 100H
; ------------------ Paging END -----------------------/

; ------------------- CR registers --------------------/
; CR0
PE_CR0		equ 00000001H
PG_CR0		equ 80000000H
WP_CR		equ 00010000H
; CR3
PWT_CR3		equ 00000008H
PCD_CR3		equ 00000010H
; ----------------------- CR END ----------------------/

; ==================  Constant  END ====================



; ======================= Macro ========================
; 1 base address
; 2 limit
; 3 attributes
%macro descriptor 3
	dw %2 & 0FFFFH
	dw %1 & 0FFFFH
	db (%1 >> 16) & 0FFH
	dw (%3 & 0F0FFH) | ((%2 >> 8) & 0F00H)
	db (%1 >> 24) & 0FFH
%endmacro

; 1 offset
; 2 selector
; 3 dcount
; 4 attribute
%macro gate 4
	 dw %1 & 0FFFFH
	 dw %2 & 0FFFFH
	 dw (%3 & 1FH) | ((%4 << 8) & 0FF00H)
	 dw (%1 >> 16) & 0FFFFH
%endmacro


; ===================== Macro END ======================


