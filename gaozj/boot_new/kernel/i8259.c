#include"type.h"

static struct hw_interrupt_type i8259A_irq_type = {
	"XT-PIC",
	startup_8259A_irq;
	shutdown_8259A_irq;
	enable_8259A_irq;
	disable_8259A_irq;
	ack_8259A_irq;
	end_8259A_irq;
	NULL;
};

void init_IRQ(void){
	int i;
	init_ISA_IRQS();
	for(i = 0; i<NR_IRQS; i++ ){
		int vector = FIRST_EXTERNAL_VECTOR + i;
		if(vector != SYSCALL_)
	}
}

void init_ISA_IRQS(void){
	int i;
	init_8259A(0);
	for(i=0; i< NR_IRQS; i++){
		irq_desc[i].status = IRQ_DISABLED;
		irq_desc[i].action = 0;
		irq_desc[i].depth = 1;

		if(i < 16){
			irq_desc[i].handler = &i8259A_irq_type; 
		}
		else 
		  irq_desc[i].handler = &no_irq_type;
	}
}

//init 8259 
void init_8259A(int auto_eoi){
	outb(0x20, 0x11);	//8259-1 ICW1
	outb(0xa0, 0x11);	//8259-2 ICW1
	
	outb(0x21, 0x20);	//8259-1 IRQ0 位于IDT向量的32个位置
	outb(0xa1, 0x28);	//8259-2 IRQ0 位于IDT向量的40个位置

	//ICW3 8259-1 IR2 <--> 8259-2
	outb(0x21, 0x04);
	outb(0xa1, 0x02);

	//ICW4
	if(auto_eoi){
		outb(0x21, 0x03);
		outb(0xa1, 0x03);	
	}
	else {
		outb(0x21, 0x01);
		outb(0xa1, 0x01);	
	}


	//OCW1-4
	//OCW1  
	outb(0x21, 0x0FF);
	outb(0xA1, 0x0FF);
}


	u_int startup_8259A_irq(u_int irq){
	}

	u_int shutdown_8259A_irq(u_int irq){
	}

	u_int enable_8259A_irq(u_int irq){
	}

	u_int disable_8259A_irq(u_int irq){
	}

	u_int ack_8259A_irq(u_int irq){
	}
	
	u_int end_8259A_irq(u_int irq){
	}
