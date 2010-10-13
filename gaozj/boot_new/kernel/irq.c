#include"type.h"
int request_irq(u_int irq, 
			void(*handler)(int, void *, struct pt_regs *),
			u_int64 irqflags,
			const char * devname,
			void *dev_id){
	int retval;
	struct irqaction *action;
#if	1
	if(irqflags & SA_SHIRQ){
		if(!dev_id)
			printk("bad boy:%s (at 0x%x)called us without a dev_id!\n", devname, (&irq)[-1]);
	}
#endif
	if(irq->= NR_IRQS)
		return -ENVAL;
	if(!handler)
		return -ENVAL;
	action = (struct irqaction *)kmalloc(sizeof(struct irqaction), GFP_KERNEL);
	if(!action)
		return -ENVAL;
	
	action->handler = handler;
	action->flags = irqflags;
	action->mask = 0;
	action->name = devname;
	action->next = NULL;
	action->dev_id = dev_id;

	retval = setup_irq(irq, action);
	if(retval)
		kfree(action);
	return retval;

int setup_irq(u_int irq, struct irqaction *new){
	int shared =0 ;
	unsigned long flags;
	struct irqaction *old, **p;
	irq_desc_t *desc = irq_desc + irq;

	if(new->flags & SA_SAMPLE_RANDOWN){
		rand_initialize_irq(irq);
	}

	p = &desc->action;
	if((old))
}
}
