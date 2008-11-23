#include "proto.h"
#include "const.h"
#include "type.h"
#include "protect.h"
#include "process.h"

#define P_EXEC_ONLY (P_FLAG | S_FLAG | EXEC_ONLY)
#define P_RW_DATA (P_FLAG | S_FLAG | RW)

extern int curr_pos;
extern t_8 gdt_ptr[6];
extern descriptor gdt[GDT_SIZE];
extern t_8 idt_ptr[6];
extern gate idt[IDT_SIZE];
extern tss tss0;
extern t_32 int_re_enter;
extern process *p_curr_proc;
extern process proc_list[NR_TASKS];
extern char process_stack[TOTAL_STACK_SIZE];
extern init_proc_info init_proc_table[NR_TASKS];

void process0() {
	int i = 0;
	while (1) {
		get_ticks();
		disp_str("A");
		disp_int(i++);
		disp_str(".");
		delay(100000);
	};
}

void process1() {
	int i = 0x100;
	while (1) {
		disp_str("B");
		disp_int(i++);
		disp_str(".");
		delay(100000);
	};
}

int aphrodite_main() {

	disp_str("--------\"aphrodite_main\" started----------\n");
	init_proc_info *p_info = init_proc_table;	
	process *p_proc = proc_list;
	char *p_stack = process_stack + TOTAL_STACK_SIZE;
	t_16 selector_ldt = FIRST_LDT_SEL;
	int i = 0;
	for (; i < NR_TASKS; i++) {
		strcpy(p_proc->p_name, p_info->name);
		p_proc->pid = i;
		p_proc->ldt_sel = selector_ldt;
		/* Copy LDT0*/
		memcpy(&p_proc->ldt[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(descriptor));
		p_proc->ldt[0].attr1 = P_EXEC_ONLY | (PRIVILEGE_SERVICE << 5);
	 	
		/* Copy LDT1*/
		memcpy(&p_proc->ldt[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(descriptor));
		p_proc->ldt[1].attr1 = P_RW_DATA | (PRIVILEGE_SERVICE << 5);

		p_proc->regs.cs = ((8 * 0) & SEL_RPL_MASK & SEL_TI_MASK) | RPL_SERVICE | SEL_FROM_LDT;
		p_proc->regs.ds = ((8 * 1) & SEL_RPL_MASK & SEL_TI_MASK) | RPL_SERVICE | SEL_FROM_LDT;
		p_proc->regs.es = ((8 * 1) & SEL_RPL_MASK & SEL_TI_MASK) | RPL_SERVICE | SEL_FROM_LDT;
		p_proc->regs.fs = ((8 * 1) & SEL_RPL_MASK & SEL_TI_MASK) | RPL_SERVICE | SEL_FROM_LDT;
		p_proc->regs.ss = ((8 * 1) & SEL_RPL_MASK & SEL_TI_MASK) | RPL_SERVICE | SEL_FROM_LDT;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SEL_RPL_MASK) | RPL_SERVICE;
		p_proc->regs.eip = (t_32)p_info->eip;
		p_proc->regs.esp = (t_32)p_stack;
		p_proc->regs.eflags = 0x1202;

		p_stack -= p_info->stack_size;
		p_proc++;
		p_info++;
		selector_ldt += 1 << 3;
	}
	
	int_re_enter = -1;
	p_curr_proc = proc_list;
	
	put_irq_handler(CLOCK_IRQ, clock_handler);
	enable_irq(CLOCK_IRQ);
	restart();
	while (1) {}

	return 0;
}

