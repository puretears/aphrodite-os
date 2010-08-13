#ifndef IRQ_H
#define IRQ_H

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

struct __cacheline_aligned irq_desc {
	unsigned int status;
	hw_irq_controller *handler;
};

typedef struct irq_desc irq_desc_t;
#endif
