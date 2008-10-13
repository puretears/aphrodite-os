#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proto.h"

extern int curr_pos;
extern t_8 gdt_ptr[6];
extern descriptor gdt[GDT_SIZE];
extern t_8 idt_ptr[6];
extern gate idt[IDT_SIZE];

void cstart() {
	
	disp_str("\n\n\n\n\n\n\n\n\n===================cstart===========\n");

	memcpy(&gdt, (void*)(*((t_32 *)(&gdt_ptr[2]))), 
		(*((t_16 *)(&gdt_ptr[0])) + 1));

	t_16 *p_gdt_limit = (t_16 *)(&gdt_ptr[0]);
	t_32 *p_gdt_base = (t_32 *)(&gdt_ptr[2]);
	t_16 *p_idt_limit = (t_16 *)(&idt_ptr[0]);
	t_32 *p_idt_base = (t_32 *)(&idt_ptr[2]);

	/* Set new gdt_ptr*/
	*p_gdt_limit = GDT_SIZE * sizeof(descriptor);
	*p_gdt_base = (t_32)&gdt;
	*p_idt_limit = IDT_SIZE * sizeof(gate);
	*p_idt_base = (t_32)&idt;

	init_idt();
	
	disp_str("===================finished!=================\n");

		
}
