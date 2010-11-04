#include "linux/system.h"
#include "asm/segment.h"
#include "linux/irq.h"
#include "linux/io.h"

#define NR_IRQS 224 // 256 - 32 = 224. 0~31 is reserved by Intel

static inline void set_interrupt_gate(int vector, void *offset) {
	set_gate(&idt[vector], 0, 14, offset);
}

unsigned int startup_8259A_irq(unsigned int);
void enable_8259A_irq(unsigned int);
void disable_8259A_irq(unsigned int);
void mask_and_ack_8259A_irq(unsigned int);
void end_8259A_irq(unsigned int);
#define shutdown_8259A_irq disable_8259A_irq

/* Interrupt vector 0x20 - 0x2F*/
static hw_irq_controller i8259A_irq_type = {
	"IA32_PIC",
	startup_8259A_irq,
	shutdown_8259A_irq,
	enable_8259A_irq,
	disable_8259A_irq,
	mask_and_ack_8259A_irq,
	end_8259A_irq
};

unsigned int startup_none(unsigned int i) { };
void shutdown_none(unsigned int i) {};
void enable_none(unsigned int i) {};
void disable_none(unsigned int i) {};
void ack_none(unsigned int i) {};
void end_none(unsigned int i) {};

/* Interrupt vector 0x30 - 0xFF*/
static hw_irq_controller no_irq_type = {
	"None_PIC",
	startup_none,
	shutdown_none,
	enable_none,
	disable_none,
	ack_none,
	end_none
};

struct irq_desc_t irq_desc_table[NR_IRQS] = {
	[0 ... (NR_IRQS - 1)] = { 0, &no_irq_type }
};
#define SYMBOL_NAME_STR(x) x

#define IRQ(x, y) \
	IRQ##x##y##_interrupt

#define IRQLIST_16(x) \
	IRQ(x, 0), IRQ(x, 1), IRQ(x, 2), IRQ(x, 3), \
	IRQ(x, 4), IRQ(x, 5), IRQ(x, 6), IRQ(x, 7), \
	IRQ(x, 8), IRQ(x, 9), IRQ(x, a), IRQ(x, b), \
	IRQ(x, c), IRQ(x, d), IRQ(x, e), IRQ(x, f)



#define BI(x, y) BUILD_IRQ(x##y)

#define BUILD_16_IRQS(x) \
	BI(x, 0) BI(x, 1) BI(x, 2) BI(x, 3) \
	BI(x, 4) BI(x, 5) BI(x, 6) BI(x, 7) \
	BI(x, 8) BI(x, 9) BI(x, a) BI(x, b) \
	BI(x, c) BI(x, d) BI(x, e) BI(x, f)

#define IRQ_NAME2(nr) nr##_interrupt()
#define IRQ_NAME(nr) IRQ_NAME2(IRQ##nr)


#define BUILD_IRQ(nr) \
	void IRQ_NAME(nr); \
	__asm__( \
		"\n" \
		".align 16\n" \
		"IRQ"#nr"_interrupt:\n\t" \
		"pushl $"#nr"-256\n\t" \
		"jmp common_interrupt");

#define BUILD_COMMON_IRQ() \
	void call_do_IRQ(void); \
	__asm__( \
		"\n" \
		".align 16\n" \
		"common_interrupt:\n\t" \
		SAVE_ALL \
		"pushl $ret_from_intr\n\t" \
		"call_do_IRQ:\n\t" \
		"jmp do_IRQ");

unsigned int do_IRQ(struct pt_regs regs) {
	int irq_no = regs.orig_eax & 0xFF;
	struct irq_desc_t *desc = &irq_desc_table[irq_no];

	desc->handler->ack(irq_no);
};

#define SAVE_ALL \
	"cld\n\t" \
	"pushl %es\n\t" \
	"pushl %ds\n\t" \
	"pushl %eax\n\t" \
	"pushl %ebp\n\t" \
	"pushl %edi\n\t" \
	"pushl %esi\n\t" \
	"pushl %edx\n\t" \
	"pushl %ecx\n\t" \
	"pushl %ebx\n\t" \
	"movl $0x18, %edx\n\t" \
	"movw %dx, %ds\n\t" \
	"movw %dx, %es\n\t"

BUILD_16_IRQS(0x0)
BUILD_COMMON_IRQ()

void (*interrupt[NR_IRQS])(void) = { IRQLIST_16(0x0), };

__asm__("ret_from_intr:");
#undef IRQ
#undef IRQLIST_16

#define FIRST_EXTERNAL_VECTOR 0x20
#define SYSCALL_VECTOR 0x80


#define __byte(x, y) (((unsigned char *)&(y))[x])
#define cached_21 (__byte(0, cached_irq_mask))
#define cached_A1 (__byte(1, cached_irq_mask))

static unsigned int cached_irq_mask = 0xFFFF;



void init_8259A(int auto_eoi) {
	outb(0x21, 0xFF);
	outb(0xA1, 0xFF);
	
	// 8259A Master Init
	// ICW1: need ICW4, cascade mode, edge trigged mode
	outb(0x20, 0x11);
	io_wait();
	// ICW2: vector from 0x20, 0x27
	outb(0x21, 0x20);
	io_wait();
	// ICW3: slave has on IR2
	outb(0x21, 0x4);
	io_wait();
	// ICW4:
	if (auto_eoi)
		outb(0x21, 0x03); // 8086mode, auto eoi
	else
		outb(0x21, 0x01); // 8086mode, normal eoi
	io_wait();

	// 8259A Slave Init
	outb(0xA0, 0x11);
	io_wait();
	outb(0xA1, 0x28);
	io_wait();
	outb(0xA1, 0x2);
	io_wait();
	outb(0xA1, 0x01);
	io_wait();

	if (auto_eoi)
		i8259A_irq_type.ack = disable_8259A_irq;
	else
		i8259A_irq_type.ack = mask_and_ack_8259A_irq;

	io_wait();
	io_wait();
	io_wait();
	io_wait();

	outb(0x21, cached_21);
	outb(0xA1, cached_A1);
}

#define IRQ_DISABLED 0
#define IRQ_INPROGRESS 1

void init_ISA_irqs() {
	init_8259A(0);

	int i = 0;
	for(; i < NR_IRQS; i++) {
		irq_desc_table[i].status = IRQ_DISABLED;
		if (i < 16) {
			irq_desc_table[i].handler = &i8259A_irq_type; 
		}	
		else {
			irq_desc_table[i].handler = &no_irq_type;
		}
	}
}

void init_IRQ(void) {
	int i = 0, vector = 0;

	init_ISA_irqs();
	/* Initialize IDT 0x20 ~ 0xFF*/
	for(; i < NR_IRQS; i++) {
		vector = FIRST_EXTERNAL_VECTOR + i;
		if (vector != SYSCALL_VECTOR) {
			set_interrupt_gate(vector, interrupt[i]);
		}
	}
	/*TODO:
	 * System clock and FPU intr have not been initialized.
	 * */
}

void enable_8259A_irq(unsigned int irq) {
	unsigned int mask = ~(1 << irq);

	//TODO: The following operation should be atomic.
	cached_irq_mask &= mask;
	if (irq & 8)
		outb(0xA1, cached_A1);
	else
		outb(0x21, cached_21);
}

void disable_8259A_irq(unsigned int irq) {
	unsigned int mask = (1 << irq);

	//TODO: The following operation should be atomic.
	cached_irq_mask |= mask;
	if (irq & 8)
		outb(0xA1, cached_A1);
	else
		outb(0x21, cached_21);
}


unsigned int startup_8259A_irq(unsigned int irq) {
	enable_8259A_irq(irq);
	return 0;
}

void end_8259A_irq(unsigned int irq) {
	if(!(irq_desc_table[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))	{
		enable_8259A_irq(irq);
	}
}

/*TODO:
 * Mask and ACK 8259A request.*/
unsigned int irq_err_count = 0;

void mask_and_ack_8259A_irq(unsigned int irq) {
	unsigned int irqmask = 1 << irq;

	if (cached_irq_mask & irqmask)
		goto spurious_8259A_irq;

	cached_irq_mask |= irqmask;

handle_real_irq:
	if (irq & 8) {
		outb(0xA1, cached_A1);
		outb(0xA0, 0x60 + (irq & 7));
		outb(0xA0, 0x62);
	}
	else {
		outb(0x21, cached_21);
		outb(0x20, 0x60 + irq);
	}

	return;

spurious_8259A_irq:
	if (i8259A_irq_real(irq))
		goto handle_real_irq;

	static int spurious_irq_mask;

	if (!(spurious_irq_mask & irqmask)) {
		printk_new("Spurious 8259A interrupt: IRQ%4d.\n", irq);
		spurious_irq_mask |= irqmask;
	}

	irq_err_count++;
	goto handle_real_irq;
}

int i8259A_irq_real(unsigned int irq) {
	int isr_val = 0;
	int irqmask = 1 << irq;

	if (irq < 8) { // From master
		outb(0x20, 0x0B);
		isr_val = inb(0x20) & irqmask;
		outb(0x20, 0x0A);
	}
	else {
		outb(0xA0, 0x0B);
		isr_val = inb(0xA0) & (irqmask >> 8);
		outb(0xA0, 0x0A);
	}
	
	return isr_val;
}

int i8259A_irq_pending(unsigned int irq) {
	unsigned int irqmask = 1 << irq;
	int irr_val;

	if (irq < 8) {
		irr_val = inb(0x20) & irqmask;
	}
	else {
		irr_val = inb(0xA0) & (irqmask >> 8);
	}

	return irr_val;
}
