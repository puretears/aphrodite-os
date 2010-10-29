#include "type.h"

/*
 * IRQ line status.
 */
#define IRQ_INPROGRESS	1	/* IRQ handler active - do not enter! 一处处理程序正在执行 */
#define IRQ_DISABLED	2	/* IRQ disabled - do not enter! 设备驱动程序禁用的IRQ*/
#define IRQ_PENDING	4	/* IRQ pending - replay on enable 已经在线上的IRQ*/
#define IRQ_REPLAY	8	/* IRQ has been replayed but not acked yet */
#define IRQ_AUTODETECT	16	/* IRQ is being autodetected */
#define IRQ_WAITING	32	/* IRQ not yet seen - for autodetection */
#define IRQ_LEVEL	64	/* IRQ level triggered */
#define IRQ_MASKED	128	/* IRQ masked - shouldn't be seen again */
#define IRQ_PER_CPU	256	/* IRQ is per CPU 8086CPU 没有使用*/

#define IRQ_NAME2(nr) nr##_interrupt(void)
#define IRQ_NAME(nr) IRQ_NAME2(IRQ##nr)
#define BUILD_IRQ(nr)\
	asmlinkage void IRQ_NAME(nr);\
__asm__(\
"\n"__ALIGN_STR"\n" \
SYMBOL_NAME_STR(IRQ) #nr "_interrupt:\n\t" \
			"pushl $"#nr"-256\n\t"\
			"jmp common_interrupt");
			
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
}irq_desc_t;a


//当中断发生时自动
struct irqaction{
	void (*handler)(int, void *, struct pt_regs *);
	u_long flags;
	u_long mask;
	const char *name;
	void *dev_id;
	struct irqaction *next;	
};
