#include "type.h"
#include "ptrace.h"
#include "print.h"
#include "segment.h"

// Definite in sys_call.s
void divide_error();

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

	int ss = KERNEL_DS, esp = (int)&p_regs->old_esp;

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
	printk_new("ds: %4x    es:%4x    fs:%4x    gs:%4x\n",
			p_regs->ds, p_regs->es, p_regs->fs, p_regs->gs);
	printk_new("\n");
	/*TODO: 
	 * We should get task index here to dump CURRENT process stack and code.*/
	return;
}

DO_ERROR(0, "Divide error", divide_error)
