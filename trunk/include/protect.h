#ifndef PROTECT_H
#define PROTECT_H
#include "type.h"

// Segment descriptor
typedef struct _descriptor {
	t_16 limit_low;
	t_16 base_addr_low;
	t_8 base_addr_mid;
	t_8 attr1;
	t_8 attr2_limit_high;
	t_8 base_addr_high;
}descriptor;

// Gate descriptor
typedef struct _gate {
	t_16 offset_low;
	t_16 selector;
	t_8 dcount;	// Only valid for call gate
	t_8 attr;
	t_16 offset_high;
}gate;

/* GDT index*/
#define DUMMY_INDEX 0
#define FLAT_C_INDEX 1
#define FLAT_D_INDEX 2
#define VEDIO_INDEX 3
#define TSS_INDEX 4
#define FIRST_LDT_INDEX 5

/* Selector*/
#define DUMMY_SEL 0
#define FLAT_C_SEL 0x08
#define FLAT_D_SEL 0x10
#define VEDIO_SEL 0x1B
#define TSS_SEL  0x20
#define FIRST_LDT_SEL 0x28

#define SELECTOR_KERNEL_CS FLAT_C_SEL
#define SELECTOR_KERNEL_DS FLAT_D_SEL
#define SELECTOR_KERNEL_GS VEDIO_SEL

#define SEL_RPL_MASK 0xFFFC
#define SEL_RPL0
#define SEL_RPL1
#define SEL_RPL2
#define SEL_RPL3

#define SEL_TI_MASK 0xFFFB
#define SEL_FROM_GDT 0
#define SEL_FROM_LDT 4

#define LDT_SIZE 2

/* Segment descriptor attributes*/
#define G_FLAG 0x8000
#define DB_FLAG 0x4000
#define P_FLAG 0x0080
#define S_FLAG 0X10

#define DPL0 0
#define DPL1 0x20
#define DPL2 0x40
#define DPL3 0x40

/* System segment types*/
#define AVAILIABLE_TSS16 1
#define LDT 2
#define BUSY_TSS16 3
#define CALL_GATE16 4
#define TASK_GATE 5
#define INTERRUPT_GATE16 6
#define TRAP_GATE16 7
// 8 is reserved
#define AVAILABLE_TSS32 9
// 10 is reserved
#define BUSY_TSS32 0xB
#define CALL_GATE32 0xC
// D is reserved
#define INTERRUPT_GATE32 0xE
#define TRAP_GATE32 0xF

/* Data and code segment types*/
/* Data*/
#define R_ONLY 0
#define R_ONLY_A 1
#define RW 2
#define RW_A 3
#define R_ONLY_ED 4
#define R_ONLY_ED_A 5
#define RW_ED 6
#define RW_ED_A 7
/* System*/
#define EXEC_ONLY 8
#define EXEC_ONLY_A 9
#define EXEC_R 10
#define EXEC_R_A 11
#define EXEC_ONLY_C 12
#define EXEC_ONLY_C_A 13
#define EXEC_R_C 14
#define EXEC_R_C_A 15

/* Interrupt vector */
#define INT_DIVIDE_ERROR 0
#define INT_RESERVED 1
#define INT_NMI_INTERRUPT 2
#define INT_BREAKPOINT 3
#define INT_OVERFLOW 4
#define INT_BOUND_RANGE_EXCEED 5
#define INT_INVALID_OPCODE 6
#define INT_DEV_NOT_AVAILABLE 7
#define INT_DOUBLE_FAULT 8
#define INT_COPROCESSOR_SEG_OVERRUN 9
#define INT_INVALID_TSS 10
#define INT_SEG_NOT_PRESENT 11
#define INT_STACK_SEG_FAULT 12
#define INT_GENERAL_PROTECTION 13
#define INT_PAGE_FAULT 14
#define INT_RESERVED1 15
#define INT_X87_FPU_FAULT 16
#define INT_ALIGNMENT_CHECK 17
#define INT_MACHINE_CHECK 18
#define INT_XF_SMID_EXCEPTION 19

/* 8259A*/
#define INTERRUPT_IRQ0 0x20
#define INTERRUPT_IRQ8 0x28

#endif
