#include "const.h" // For port number
#include "type.h"  // For type definition
#include "protect.h" // For IRQ number
#include "proto.h" // For port IO funcitons

void init_8259A() {
	// ICW1
	write_to_port(INT_MASTER1, 0x11);
	write_to_port(INT_SLAVE1, 0x11);
	// ICW2
	write_to_port(INT_MASTER2, INTERRUPT_IRQ0);
	write_to_port(INT_SLAVE2, INTERRUPT_IRQ8);
	// ICW3
	write_to_port(INT_MASTER2, 4);
	write_to_port(INT_SLAVE2, 2);
	// ICW4
	write_to_port(INT_MASTER2, 1);
	write_to_port(INT_SLAVE2, 1);

	// OCW1 disable all interrupts
	write_to_port(INT_MASTER2, 0xFE); // Only enable keyboard interrupt
	write_to_port(INT_SLAVE2, 0xFF);
};

void spurious_irq(int irq) {
	disp_str("spurious_irq: ");
	disp_int(irq);
	disp_str("\n");
}
