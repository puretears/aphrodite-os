#include"type.h"
#include"ptrace.h"

int handle_IRQ_event(unsigned int irq, struct pt_regs *regs, struct irqaction * action)
{
	int status;
	
//	irq_enter(cpu, irq)
	status = 1;
	if(!(action->flags & SA_INTERRUPT))
	__sti();	//open interrupt 
	do
	{
		status |= action->flags;
		action->handler(irq, action->dev_id, regs);
		action = action->next; 
	}
	while(action);

	if(status & SA_SAMPLE_RANDOM)
		add_interrupt_randomness(irq);

	__cli();	//close interrupt

	return status;
}

asmlinkage u_int do_IRDQ(struct pt_regs regs)
{
	int irq = irqs.orgi_eax & 0xff;
	irq_desc_t *desc = irq_desc + irq;
	struct irqaction * action;
	u_int status;
	kstat.irqs[cpu][irq]++;
	desc->handler->ack(irq);
	status = desc->status & ~(IRQ_REPLAY | IRQ_WAITING);
	status |= IRQ_PENDING;

	/*If the IRQ is disable for whatever reason ,we connot use the action we have
		 */
	action = NULL;
	if(!(status & (IRQ_DISABLE | IRQ_INPROGRESS)))
	{
		action = desc->action;
		action &= ~IRQ_PENDING;
		action |= IRQ_INPROGRESS;
	}
	desc->status = status;
	if(!action)
		goto out;

	for(;;)
	{
		handle_IRQ_EVENT(irq, &regs, action);
		if(!(desc->staus & IRQ_PENDING))
			break;
		desc->status &= ~IRQ_PENDING;
	}
	desc->status &= ~IRQ_INPROGRESS;
out:
	desc->handler->end(irq);

}

int request_irq(u_int irq, void(*handler)(int, void *, struct pt_regs *), 
			unsigned long irqflags,
			const char *devname,
			void *dev_id)
{
	int retval;
	struct irqaction * action;
	if(irqflags & SA_SHIRQ)
		if(!dev_id)
			printk("Bad boy: %s(at 0x%x)called us without a dev_id!\n",devname,(&irq)[-1]);
	if(irq >= NR__IRQS)
		return -EINVAL;
	if(!handler)
		return -EINVAL;

	action = (struct irqaction *)
		kmalloc(sizeof(struct irqaction), GFP_KERNEL);
	if(!action)
		return -EINVAL;

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
}

int setup_irq(u_int irq, struct irqaction *new)
{
	int shared = 0;
	unsigned long flags;
	struct irqaction *old, **p;
	irq_desc_t *desc = irq_desc + irq;

	if(new->flags & SA_SAMPLE_RANDOM)
	{
		rand_initialize_irq(irq);
	}
	p = &desc->action;
	if((old = *p)!=NULL)
	{
		do
		{
			p = &old->next;
			old = *p;
		}while(old);
		shared = 1;
	}
	*p = new;
	if(!shared)
	{
		desc->depth = 0;
		desc->status &= ~(IRQ_DISABLED | IRQ_AUTODETECT |IRQ_WAITLING);
		desc->handler->startup(irq);
	}
//	register_irq_proc(irq);
	return 0;
}
