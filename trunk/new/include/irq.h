#ifndef IRQ_H
#define IRQ_H

#include "spinlock.h"

struct hw_interrupt_type {
	const char *pic_name;
	unsigned int (*startup)(unsigned int irq);
	void (*shutdown)(unsigned int irq);
	void (*enable)(unsigned int irq);
	void (*disable)(unsigned int irq);
	void (*ack)(unsigned int irq);
	void (*end)(unsigned int irq);
};

typedef struct hw_interrupt_type hw_irq_controller;

// The status of a IRQ line
// A handler for the IRQ is being executed.
#define IRQ_INPROGRESS 1
// The IRQ line has been deliberately disabled by a device driver.
#define IRQ_DISABLED   2
// An IRQ has occurred on the line and been acknoledged to the PIC.
// But hasn't been serviced by the kernel.
#define IRQ_PENDING    4
// The IRQ line has been disabled but the previous IRQ occurence hasn't
// been acknowledged to the PIC.
#define IRQ_REPLAY     8

struct irqaction {

};

struct irq_desc {
	// A set of flags describing the IRQ line status.
	unsigned int status;
	// Pointer to PIC object.
	hw_irq_controller *handler;
	// Pointer to data used by the PIC.
	void *handler_data;
	// Identify the interrupt service routine.
	irqaction *action;
	// Show 0 if the IRQ line is enabled and a positive value if
	// it has been disabled at least once.
	unsigned int depth;
	// For diagnostic only
	unsigned int irq_count;
	unsigned int irqs_unhandled;
	spinlock_t lock;
};

typedef struct irq_desc irq_desc_t;
#endif
