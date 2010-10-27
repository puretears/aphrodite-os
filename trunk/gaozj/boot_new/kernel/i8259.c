#include"type.h"
#include"irq.h"

#define BI(x,y) \
	BUILD_IRQ(x##y)
#define BUILD_16_IRQS(x) \
	BI(x,0) BI(x,1) BI(x,2) BI(x,3) \
	BI(x,4) BI(x,5) BI(x,6) BI(x,7) \
	BI(x,8) BI(x,9) BI(x,a) BI(x,b) \
	BI(x,c) BI(x,d) BI(x,e) BI(x,f)
BUILD_16_IRQS(0x0)
	
#define IRQ(x,y)\
	IRQ##x##y##_interrupt
#define IRQLIST_16(x)\
	IRQ(x,0), IRQ(x,1), IRQ(x,2), IRQ(x,3),\
	IRQ(x,4), IRQ(x,5), IRQ(x,6), IRQ(x,7),\
	IRQ(x,8), IRQ(x,9), IRQ(x,a), IRQ(x,b),\
	IRQ(x,c), IRQ(x,d), IRQ(x,e), IRQ(x,f)
void (*interrupt[NR_IRQS])(void)={
	IRQLIST_16(0x0),IRQLIST_16(0x1), IRQLIST_16(0x2), IRQLIST_16(0x3),
	arch/x86_64/kernel/i8259.c:	IRQLIST_16(0x4), IRQLIST_16(0x5), IRQLIST_16(0x6), IRQLIST_16(0x7),
	arch/x86_64/kernel/i8259.c:	IRQLIST_16(0x8), IRQLIST_16(0x9), IRQLIST_16(0xa), IRQLIST_16(0xb),
	arch/x86_64/kernel/i8259.c:	IRQLIST_16(0xc), IRQLIST_16(0xd)
};
#undef IRQ
#undef IRQLIST_16

static struct hw_interrupt_type i8259A_irq_type={
	"XT-PIC",
	startup_8259A_irq,
	shutdown_8259A_irq,
	enable_8259A_irq,
	disable_8259A_irq,
	mask_and_ack_8259A,
	end_8259A_irq,
	NULL
};
struct hw_interruput_type no_irq_type={
	.typename = "none",
	.startup = startup_none,
	.shutdown = shutdown_none,
	.enable = enable_none,
	.disable = disable_none,
	.ack = ack_none,
	.end = end_none,
	.set_affinity = NULL
};



void init_ISA_irqs(void)
{
	int i;
	init_8259A(0);
	for(i =0; i < NR_IRQS; i++)
	{
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = i;
		if(i<16)
		  irq_desc[i].handler = &i8259A_irq_type;
		else
		  irq_desc[i].handler = &no_irq_type;
	}
}

//init general irq 
void init_IRQ(void)
{
	int i ;
	init_ISA_irqs();
	for(i=0; i<NR_IRQS; i++)
	{
		int vector = FIRST_EXTERNAL_VECTOR + i;
		if(vector != SYSCALL_VECTOR)	//except system_call 128
			set_inir_gate(vector, interrupt[i]);
	}
	outb_p(0x34, 0x34);
	outb_p(LATCH & 0xff, 0x40);
	outb(LATCH >> 8, 0X40);
#ifndef CONFIG_VISWS
	setup_irq(2, &irq2);
#endif 
	if(boo_cpu_data.hard_match && !cpu_has_fpu)
	  setup_irq(13, &irq13);
}
