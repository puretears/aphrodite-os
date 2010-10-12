/*
 8259A PIC的基本原理  
 8259A PIC芯片的基本逻辑块图如下所示：  
 “中断屏蔽寄存器”（Interrupt Mask Register，简称IMR）用于屏蔽8259A的中断信号输入，每一位对应一个输入。当IMR中的bit(0≤i≤7)位被置1时，相对应的中断信号输入线IRi上的中断信号将被8259A所屏蔽，也即IRi被禁止。  
 当外设产生中断信号时（由低到高的跳变信号，80x86系统中的8259A是边缘触发的，Edge Triggered），中断信号被输入到“中断请求寄存器”（Interrupt Request Register，简称IRR），并同时看看IMR中的相应位是否已被设置。如果没有被设置，则IRR中的相应位被设置为1，表示外设产生一个中断请求，等待CPU服务。  
 然后，8259A的优先级仲裁部分从IRR中选出一个优先级最高中断请求。优先级仲裁之后，8259A就通过其INT引脚向CPU发出中断信号，以通知CPU有外设请求中断服务。CPU在其当前指令执行完后就通过他的INTA引脚给8259A发出中断应答信号，以告诉8259A，CPU已经检测到有中断信号产生。  
 8259A在收到CPU的INTA信号后，将优先级最高的那个中断请求在ISR寄存器（In－Service Register，简称ISR）中对应的bit置1，表示该中断请求已得到CPU的服务，同时IRR寄存器中的相应位被清零重置。  
 然后，CPU再向8259A发出一个INTA脉冲信号，8259A在收到CPU的第二个INTA信号后，将中断请求对应的中断向量放到数据总线上，以供CPU读取。CPU读到中断向量后，就可以装入执行相应的中断处理程序。  
 如果8259A工作在AEOI（Auto End Of Interrupt，简称AEOI）模式下，则当他收到CPU的第二个INTA信号时，它就自动重置ISR寄存器中的相应位。否则，ISR寄存器中的相应位就一直保持为1，直到8259A显示地收到来自于CPU的EOI命令。 
*/
#include"type.h"
#include"io.h"


//将16位cached_irq_mask分成两个8位的变量
#define __byte(x,y) (((unsigned char *)&(y))[x])
#define cached_A1(__byte(0, cached_irq_mask))
#define cached_21(__byte(1, cached_irq_mask))
#define cached_irq_mask = 0xffff;

#define BI(x,y)	BUILD_IRQ(x##y)
#define BUILD_16_IRQ(x)\
	BI(x,0) BI(x,1) BI(x,2) BI(x,3) \
	BI(x,4) BI(x,5) BI(x,6) BI(x,7)\
	BI(x,8) BI(x,9) BI(x,a) BI(x,b)\
	BI(x,c)	BI(x,d)	BI(x,e)	BI(x,f)
BUILD_16_IRQ(0x0)

#define IRQ##x##y##_interrupt
#define IRQLIST_16(x)\
	IRQ(x, 0), IRQ(x, 1), IRQ(x, 2), IRQ(x, 3)\
	IRQ(x, 4), IRQ(x, 5), IRQ(x, 6), IRQ(x, 7)\
	IRQ(x, 8), IRQ(x, 9), IRQ(x, a), IRQ(x, b)\
	IRQ(x, c), IRQ(x, d), IRQ(x, e), IRQ(x, f)
void (*interrupt[NR_IRQS])(void) = {
	IRQLIST_16(0x0),
#ifdef CONFIG_X86_IO_APIC 
	IRQLIST_16(0x1), IRQLIST_16(0x2), IRQLIST_16(0x3), IRQLIST_16(0x4),
	IRQLIST_16(0x5), IRQLIST_16(0x6), IRQLIST_16(0x7), IRQLIST_16(0x8),
	IRQLIST_16(0x9), IRQLIST_16(0xa), IRQLIST_16(0xb), IRQLIST_16(0xc),
	IRQLIST_16(0xd)
#endif
};

#undef IRQ
#undef IRQLIST_16

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


	u_int enable_8259A_irq(u_int irq){
		u_int mask = ~(1<<irq);
		cached_irq_mask &= mask;
		if(irq & 8)
			outb(0xa1, cached_A1);
		else 
			outb(0x21, cached_21);		  
	}

//disable_8259A 通过将IMR相应的相应位置1来实现
	u_int disable_8259A_irq(u_int irq){
		u_int mask = 1 << irq;
		cached_irq_mask |= mask;
		if(irq & 8)
		  outb(0xA1, cached_A1);
		else 
		  outb(0x21, cached_21);
	}

//屏蔽该位8259中断,以及发送eoi信号
/*
函数mask_and_ack_8259A()用来向主、从8259A发送EOI命令。向8259A应答因该遵循这样的顺序：首先，屏蔽相应的IRQ；然后，发送EOI命令。此外，如果IRQ来自于从8259A，还必须先向Slave 8259A发送EOI命令，再向Master 8259A发送EOI命令。如果IRQ来自于Master 8259A，则仅仅向Master 8259A发送EOI命令就可以了。
*/
	u_int mask_and_ack_8259A_irq(u_int irq){
		u_int mask = 1 << irq;
		if(cached_irq_mask & mask)		//判断参数irq所指定的IRQ线是否处于屏蔽状态,如果已经被屏蔽，则说明8259A在IMR寄存器中的相应位被置1的情况下仍然向CPU发出了相应的中断信号，因此这是一个假的中断，所以跳转到spurious_8259A_irq部分进行处理。
		  goto spuirous_8259_irq;
		cached_irq_mask |= mask;	//将cached_irq_mask 的相应位置1
		
handle_real_irq:
		if(irq & 8){					//slave 8259A
			outb(0xA1, cached_A1);
			outb(0xA0, 0x60 + (irq & 7));	//specific EOI, 对irq&7 进行操作
			outb(0xA0, 0x62);				//向master 8259A 写入ocw2，其值为0x62 （从8259A 通过 主8259A 的IR2进行级联）
		}
		else {
			outb(0x21, cached_21);
			outb(0x20, 0x60 + irq);		//specific EOI
		}
		return ;
spurious_8259_irq:				
		if(i8259A_irq_real(irq))
		  goto handle_real_irq;

		static int spurious_irq_mask;

		if(!(spurious_irq_mask & irqmask)){
			printk(KERN_DEBUG "squrious 8259A interrupt: IRQ%d\n", irq);	//printk messages
			spurious_irq_mask |= irqmask;	
		}
		irq_err_count++;
		goto handle_real_irq;
	}
	
	u_int end_8259A_irq(u_int irq){
		if(!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS)))
		  enable_8259A_irq(irq);
	}
	
//判断是否为真正的IRQ请求
	int	i8259A_irq_real(u_int irq){
		int isr_val = 0;
		int irqmask = 1 << irq;
		
		if(irq < 8){
			outb(0x20, 0x0B);
			value = inb(0x20) & irqmask;
			outb(0x20, 0xA0);				// back to the IRR register
			return value;
		}
		else {
			outb(0xA0, 0xB0);
			value = inb(0xA0) & (irqmask >> 8);
			outb(0xA0, 0xA0);				// back to the IRR register
			return value;	
		}
	}

