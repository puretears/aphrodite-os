#ifndef PROCESS_H
#define PROCESS_H

#include "type.h"
#include "protect.h"

#define NR_TASKS 2
#define PROCESS0_STACK_SIZE 0x8000
#define PROCESS1_STACK_SIZE 0x8000
#define TOTAL_STACK_SIZE (PROCESS0_STACK_SIZE + PROCESS1_STACK_SIZE)

typedef struct {
	t_32 gs;
	t_32 fs;
	t_32 es;
	t_32 ds;
	t_32 edi;
	t_32 esi;
	t_32 ebp;
	t_32 kernel_esp;
	t_32 ebx;
	t_32 edx;
	t_32 ecx;
	t_32 eax;
	t_32 retaddr;
	t_32 eip;
	t_32 cs;
	t_32 eflags;
	t_32 esp;
	t_32 ss; /* Interrupted procedure's stack*/
} stack_frame;

typedef struct {
	t_32 previous_link;
	t_32 esp0;
	t_32 ss0;
	t_32 esp1;
	t_32 ss1;
	t_32 esp2;
	t_32 ss2;
	t_32 cr3;
	t_32 eip;
	t_32 eflags;
	t_32 eax;
	t_32 ecx;
	t_32 edx;
	t_32 ebx;
	t_32 esp;
	t_32 ebp;
	t_32 esi;
	t_32 edi;
	t_32 es;
	t_32 cs;
	t_32 ss;
	t_32 ds;
	t_32 fs;
	t_32 gs;
	t_32 ldt_sel;
	t_16 trap;
	t_16 iobase;
} tss;

typedef struct {
	t_pf_task eip;
	int stack_size;
	char name[32];
} init_proc_info;

typedef struct {
	stack_frame regs;
	t_16 ldt_sel;
	descriptor ldt[LDT_SIZE];
	t_32 pid;
	char p_name[16];
} process;

#endif
