#include "proto.h"
#include "protect.h"
#include "type.h"
#include "const.h"
#include "process.h"

#define P_INT_GATE32 (P_FLAG | INTERRUPT_GATE32)
#define P_LDT (P_FLAG | LDT)
#define P_TSS (P_FLAG | AVAILABLE_TSS32)
#define vir_to_phys(seg_base, offset) (t_32)(((t_32)seg_base + (t_32)offset))

extern int curr_pos;
extern t_8 gdt_ptr[6];
extern descriptor gdt[GDT_SIZE];
extern t_8 idt_ptr[6];
extern gate idt[IDT_SIZE];
extern tss tss0;
extern process proc_list[NR_TASKS];

void init_idt_descriptor(unsigned char vector, t_8 gate_type,
		interrupt_handler handler, unsigned char privilege);
void init_descriptor(descriptor *p_desc, t_32 base, t_32 limit, t_16 attribute);

/* Exception handlers which are defined in kernel.asm*/
void divide_error();
void single_step_error();
void nmi_interrupt();
void breakpoint();
void overflow();
void bound_range_exceeded();
void invalid_opcode();
void device_not_available();
void double_fault();
void coprocessor_segment_overrun();
void invalid_tss();
void segment_not_present();
void stack_fault();
void general_protection();
void page_fault();
void x87_fpu_floating_point_error();

/* Hardware interrupt*/
void hw_irq00();
void hw_irq01();
void hw_irq02();
void hw_irq03();
void hw_irq04();
void hw_irq05();
void hw_irq06();
void hw_irq07();
void hw_irq08();
void hw_irq09();
void hw_irq10();
void hw_irq11();
void hw_irq12();
void hw_irq13();
void hw_irq14();
void hw_irq15();

t_32 sel_to_phys(t_16 sel) {
	t_32 ret_addr;

	descriptor *p_desc = &gdt[sel >> 3];
	ret_addr = (p_desc->base_addr_low) | 
		(p_desc->base_addr_mid << 16) | 
		(p_desc->base_addr_high << 24);
}

void init_idt() {
	// Initialize 8259A
	init_8259A();
	
	/* Initialize LDT*/
	int i = 0;
	t_16 ldt_selector = FIRST_LDT_INDEX << 3;
	process *p_proc = proc_list; 		
	for (; i < NR_TASKS; i++) {
		init_descriptor(&gdt[ldt_selector >> 3],
			vir_to_phys(sel_to_phys(SELECTOR_KERNEL_DS), p_proc->ldt),
			sizeof(descriptor) * LDT_SIZE - 1,
			P_LDT);
		p_proc++;
		ldt_selector += (1 << 3);
	}

	/* Initialize IDT*/
	memset(&tss0, 0x0, sizeof(tss));
	tss0.ss0 = SELECTOR_KERNEL_DS;
	init_descriptor(&gdt[TSS_INDEX],
		vir_to_phys(sel_to_phys(SELECTOR_KERNEL_DS), &tss0),
		sizeof(tss) - 1,
		P_TSS);
	
	/* Initialize IDT*/
	init_idt_descriptor(INT_DIVIDE_ERROR, P_INT_GATE32, divide_error,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_NMI_INTERRUPT, P_INT_GATE32, nmi_interrupt,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_BREAKPOINT, P_INT_GATE32, breakpoint,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_OVERFLOW, P_INT_GATE32, overflow,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_BOUND_RANGE_EXCEED, P_INT_GATE32, bound_range_exceeded,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_INVALID_OPCODE, P_INT_GATE32, invalid_opcode,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_DEV_NOT_AVAILABLE, P_INT_GATE32, device_not_available,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_DOUBLE_FAULT, P_INT_GATE32, double_fault,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_COPROCESSOR_SEG_OVERRUN, P_INT_GATE32, 
		coprocessor_segment_overrun, PRIVILEGE_KERNEL);	
	init_idt_descriptor(INT_INVALID_TSS, P_INT_GATE32, invalid_tss,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_SEG_NOT_PRESENT, P_INT_GATE32, segment_not_present,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_STACK_SEG_FAULT, P_INT_GATE32, stack_fault,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_GENERAL_PROTECTION, P_INT_GATE32, general_protection,
		PRIVILEGE_KERNEL);
	init_idt_descriptor(INT_PAGE_FAULT, P_INT_GATE32, page_fault,
		PRIVILEGE_KERNEL);

	init_idt_descriptor(INTERRUPT_IRQ0 + 0, P_INT_GATE32, hw_irq00, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 1, P_INT_GATE32, hw_irq01, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 2, P_INT_GATE32, hw_irq02, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 3, P_INT_GATE32, hw_irq03, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 4, P_INT_GATE32, hw_irq04, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 5, P_INT_GATE32, hw_irq05, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 6, P_INT_GATE32, hw_irq06, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ0 + 7, P_INT_GATE32, hw_irq07, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 0, P_INT_GATE32, hw_irq08, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 1, P_INT_GATE32, hw_irq09, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 2, P_INT_GATE32, hw_irq10, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 3, P_INT_GATE32, hw_irq11, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 4, P_INT_GATE32, hw_irq12, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 5, P_INT_GATE32, hw_irq13, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 6, P_INT_GATE32, hw_irq14, PRIVILEGE_KERNEL);
	init_idt_descriptor(INTERRUPT_IRQ8 + 7, P_INT_GATE32, hw_irq15, PRIVILEGE_KERNEL);

	init_idt_descriptor(INT_VECTOR_SYSCALL, P_INT_GATE32, sys_call, PRIVILEGE_USER);
}


void init_idt_descriptor(unsigned char vector, t_8 gate_type, 
		interrupt_handler handler, unsigned char privilege) {
	gate *p_gate = &idt[vector];
	t_32 offset = (t_32)handler;

	p_gate->offset_low = offset & 0xFFFF;
	p_gate->selector = SELECTOR_KERNEL_CS;
	p_gate->dcount = 0;
	p_gate->attr = gate_type | (privilege << 5);
	p_gate->offset_high = (offset >> 16) & 0xFFFF;
}

void init_descriptor(descriptor *p_desc, t_32 base, t_32 limit, t_16 attribute) {
	p_desc->limit_low = limit & 0xFFFF;
	p_desc->base_addr_low = base & 0xFFFF;
	p_desc->base_addr_mid = (base >> 16) & 0xFF;
	p_desc->attr1 = attribute & 0xFF;
	p_desc->attr2_limit_high = ((limit >> 16) & 0x0F) | ((attribute >> 8) & 0xF0);
	p_desc->base_addr_high = (base >> 24) & 0xFF;
}

void exception_handler(int vector_number, int error_code,
		int eip, int cs, int eflags) {
	int i;
	int text_color = 0x74;

	char error_description[][64] = {
		"#DE divide error",
		"#DB reserved",
		"- NMI interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR BOUND Range Exceeded",
		"#UD Invalid Opcode (Undefined Opcode)",
		"#NM Device Not Available (No Math Coprocessor)",
		"#DF Double Fault",
		" Coprocessor Segment Overrun (reserved)",
		"#TS Invalid TSS",
		"#NP Segment Not Present",
		"#SS Stack-Segment Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"- (Intel reserved. Do not use!)",
		"#MF x87 FPU Floating-Point Error",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD Floating-Point Exception",
	};

	curr_pos = 0;
	for(i = 0; i < 80 * 5; i++) {
		disp_str(" ");
	}
	curr_pos = 0;

	disp_color_str("Exception! -->", text_color);
	disp_color_str(error_description[vector_number], text_color);
	disp_color_str("\n\n", text_color);
	disp_color_str("EFLAGS:", text_color);
	disp_int(eflags);
	disp_color_str("CS:", text_color);
	disp_int(cs);
	disp_color_str("EIP:", text_color);
	disp_int(eip);

	if(error_code != 0xFFFFFFFF) {
		disp_color_str("Error code:", text_color);
		disp_int(error_code);
	}
}
