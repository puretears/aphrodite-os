#include "linux/type.h"
#include "linux/ptrace.h"
#include "linux/print.h"
#include "linux/system.h"
#include "asm/segment.h"

void set_trap_gate(int vector, void *offset) {
	set_gate(&idt[vector], 0, 15, offset);
}

void set_system_gate(int vector, void *offset) {
	set_gate(&idt[vector], 3, 15, offset);
}

void set_system_intr_gate(int vector, void *offset) {
	set_gate(&idt[vector], 3, 14, offset);
}

void set_task_gate(int vector, void *offset) {
	set_gate(&idt[vector], 0, 3, 0);
}

// Definite in sys_call.s
void divide_error();
void debug();
void nmi();
void _int3();
void overflow();
void bounds();
void invalid_op();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_tss();
void segment_not_present();
void stack_segment();
void general_protection();
void page_fault();
void x87_error();
void alignment_check();
void machine_check();
void smid_exception();
void reserved();
void system_call();

// Interrupt and exception handling template
#define DO_ERROR(trap_no, err_str, name) \
void do_##name(struct pt_regs *p_regs, int err_no) { \
	die_if_kernel(err_str, p_regs, err_no); \
}

// Do nothing if we get here from Ring3.
// Dump system and die if we get here from Ring0.
void die_if_kernel(const char *err_str, struct pt_regs *p_regs, int err_no) {
	if ((p_regs->cs & 3) == 3) {
		return;
	}

	int ss = __KERNEL_DS, esp = (int)&p_regs->old_esp;

	if (p_regs->cs & 3) { // Ring1 or 2
		ss = p_regs->ss;
		esp = p_regs->old_esp;
	}
	printk_new("%s: %4d.\n", err_str, err_no);
	printk_new("EIP: %4x:%8x.\n", p_regs->cs & 0xFFFF, p_regs->eip);
	printk_new("EFLAGS: %8x.\n", p_regs->eflags);
	printk_new("eax: %8x    ebx: %8x    ecx: %8x    edx: %8x\n", 
			p_regs->eax, p_regs->ebx, p_regs->ecx, p_regs->edx);
	printk_new("esi: %8x    edi: %8x    ebp: %8x    esp: %8x\n", 
			p_regs->esi, p_regs->edi, p_regs->ebp, esp);
	printk_new("ds: %4x    es:%4x\n", p_regs->ds, p_regs->es);
	printk_new("\n");
	/*TODO: 
	 * We should get task index here to dump CURRENT process stack and code.*/
	return;
}

DO_ERROR(0, "Divide error", divide_error)
DO_ERROR(1, "Debug", debug)
DO_ERROR(3, "INT 3", int3)
DO_ERROR(4, "Overflow", overflow)
DO_ERROR(5, "Bound_range_exceeded", bounds)
DO_ERROR(6, "Invalid opcode", invalid_op)
DO_ERROR(7, "Device_not_available", device_not_available)
DO_ERROR(8, "Double fault", double_fault)
DO_ERROR(9, "Coprocessor segment overrun", coprocessor_segment_overrun)
DO_ERROR(10, "Invalid TSS", invalid_tss)
DO_ERROR(11, "Segment not present", segment_not_present)
DO_ERROR(12, "Stack segment fault", stack_segment)
DO_ERROR(13, "General protection", general_protection)
DO_ERROR(14, "Page fault", page_fault)
DO_ERROR(15, "Reserved", reserved)
// Vector 16 is implemented seperately.
DO_ERROR(17, "Alignment check", alignment_check)
DO_ERROR(18, "Machine check", machine_check)
DO_ERROR(19, "Smid exception", smid_exception)
DO_ERROR(128, "System call", system_call)
/*TODO:
 * machine_check and smid_exception not implemented yet.
 * */

void do_nmi(struct pt_regs *p, int error_no) {
	printk_new("NMI received. You probably have a hardware probelem with your RAM chips.\n");
}

void do_x87_error(struct pt_regs *p, int error_no) {
	
}

void trap_init() {
	int i;
	set_trap_gate(0, &divide_error);
	set_trap_gate(1, &debug);
	set_trap_gate(2, &nmi);
	/*int 3 - 5 can be called from ring3*/
	set_system_intr_gate(3, &_int3);
	set_system_gate(4, &overflow);
	set_system_gate(5, &bounds);
	set_trap_gate(6, &invalid_op);
	set_trap_gate(7, &device_not_available);
	set_task_gate(8, &double_fault);
	set_trap_gate(9, &coprocessor_segment_overrun); // Processor after 386 don't generate this exception
	set_trap_gate(10, &invalid_tss);
	set_trap_gate(11, &segment_not_present);
	set_trap_gate(12, &stack_segment);
	set_trap_gate(13, &general_protection);
	set_trap_gate(14, &page_fault);
	//set_trap_gate(15, &reserved);
	set_trap_gate(16, &x87_error);
	set_trap_gate(17, &alignment_check);
	set_trap_gate(18, &machine_check);
	set_trap_gate(19, &smid_exception);

	for (i = 20; i < 256; i++) {
		//set_trap_gate(i, &reserved);
	}

	set_system_gate(128, &system_call);
}
