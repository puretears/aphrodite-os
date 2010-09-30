#include "type.h"
extern irq_desc_t irq_desc[NR_IRQS];

struct hw_interrupt_type{
	const char * typename;
	u_int (*startup)(u_int irq);
	void (*shutdown)(u_int irq);
	void (*enable)(u_int irq);
	void (*disable)(u_int irq);
	void (*ack)(u_int irq);
	void (*end)(u_int irq);
	void (*set_affinity)(u_int irq, u_long mask);
};


typedef struct hw_interrupt_type hw_irq_controller;

typedef struct{
	u_int status;		//IRQ status
	hw_irq_controller * handler;
	struct irqaction *action;
	u_int depth;	
	spinlock_t lock;
}irq_desc_t;


//当中断发生时自动
struct irqaction{
	void (*handler)(int, void *, struct pt_regs *);
	u_long flags;
	u_long mask;
	const char *name;
	void *dev_id;
	struct irqaction *next;	
};
