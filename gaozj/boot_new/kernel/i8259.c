#include"type.h"
#include"io.h"

//将16位cached_irq_mask分成两个8位的变量
#define __byte(x,y) (((unsigned char *)&(y))[x])
#define cached_A1(__byte(0, cached_irq_mask))
#define cached_21(__byte(1, cached_irq_mask))
#define cached_irq_mask = 0xffff;

u_int irq_err_count = 0;

static struct hw_interrupt_type i8259A_irq_type = {
	"XT-PIC",
	startup_8259A_irq;
	shutdown_8259A_irq;
	enable_8259A_irq;
	disable_8259A_irq;
	mask_and_ack_8259A_irq;
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


	au_int startup_8259A_irq(u_int irq){
		enable_8259_irq(irq);
		return 0;
	}

	u_int shutdown_8259A_irq(u_int irq){
		
	}

/*((unsigned char *)&[0])*/
	u_int enable_8259A_irq(u_int irq){
		u_int mask = ~(1<<irq);
		cached_irq_mask &= mask;
		if(irq & 8)
			outb(0xa1, cached_irq_mask);		  
	}

	u_int disable_8259A_irq(u_int irq){
		u_int mask = 1 << irq;
		cached_irq_mask |= mask;
		if(irq & 8)
		  outb(0xA1, cached_A1);
		else 
		  outb(0x21, cached_21);
	}

	//屏蔽该位8259中断,以及发送eoi信号
	u_int mask_and_ack_8259A_irq(u_int irq){
		u_int mask = 1 << irq;
		if(cached_irq_mask & mask)
		  goto spuirous_8259_irq;
		cached_irq_mask |= mask;
		
handle_real_irq:
		if(irq & 8){
			outb(0xA1, cached_A1);
			outb(0xA0, 0x60 + (irq & 7));
			outb(0xA0, 0x62);
		}
		else {
			outb(0x21, cached_21);
			outb(0x20, 0x60 + irq);
		}
		return ;
spurious_8259_irq:
		if(i8259A_irq_real(irq))
		  goto handle_real_irq;

		static int spurious_irq_mask;

		if(!(spurious_irq_mask & irqmask)){
			printk(KERN_DEBUG "squrious 8259A interrupt: irq%d\n", irq);
			spurious_irq_mask |= irqmask;
		}
		irq_err_count++;
		goto handle_real_irq;
	}
	
//判断是否为真正的IRQ请求
	u_int end_8259A_irq(u_int irq){
	}
	
	int	i8259A_irq_real(u_int irq){
		int isr_val = 0;
		int irqmask = 1 << irq;
		
		if(irq < 8){
			outb(0x20, 0x0B);
			value = inb(0x20) & irqmask;
			outb(0x20, 0xA0);
			return value;
		}
		else {
			outb(0xA0, 0xB0);
			value = inb(0xA0) & (irqmask >> 8);
			outb(0xA0, 0xA0);
			return value;
		}
	}

