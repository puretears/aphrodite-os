#include "proto.h"
#include "protect.h"
#include "type.h"
#include "const.h"
#include "process.h"

int curr_pos = 0;
t_8 gdt_ptr[6];
descriptor gdt[GDT_SIZE];
t_8 idt_ptr[6];
gate idt[IDT_SIZE];
tss tss0;
int int_re_enter;

process *p_curr_proc;
process proc_list[NR_TASKS];
char process_stack[TOTAL_STACK_SIZE];

init_proc_info init_proc_table[NR_TASKS] = {
	{process0, PROCESS0_STACK_SIZE, "process0"},
	{process1, PROCESS1_STACK_SIZE, "process1"}
};
