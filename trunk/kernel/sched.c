#include "linux/head.h"
#include "linux/sched.h"
#include "asm/io.h"

#define PAGE_SIZE 4096
#define HZ 100
#define LATCH (1193180 / HZ)
extern unsigned char *_idt;
extern unsigned char *_gdt;
extern unsigned int pde;

#define INIT_TASK \
{ 0,\
	{\
		{ 0, 0 },\
		{ 0x9F, 0x00C0FA00 },\
		{ 0x9F, 0x00C0F200 },\
	},\
	{ \
		0, (PAGE_SIZE + (unsigned int)&init_task), 0x10, 0, 0, 0, 0,(long)&pde, 0, 0,\
		0, 0, 0, 0, 0, 0, 0, 0,\
		0x17, 0x17, 0x17, 0x17, 0x17, 0x17, _LDT(0), 0x80000000\
	}, \
}
union task_union {
	struct task_struct task;
	unsigned char stack[PAGE_SIZE];
};

static union task_union init_task = { INIT_TASK, };
struct task_struct *current = &(init_task.task);

struct task_struct *task[NR_TASKS] = { &(init_task.task), };
long user_stack[PAGE_SIZE >> 2] = { 0 };




static inline void set_tssldt_desc(unsigned char *gdt_desc, void *desc, char type) {
	__asm__("movw $104, %2\n\t"
			"movw %%ax, %3\n\t"
			"shrw $16, %%eax\n\t"
			"movb %%al, %4\n\t"
			"movb %%bl, %5\n\t"
			"movb $0, %6\n\t"
			"movb %%ah, %7"
			::"a"(desc), "b"(type), "m"(*(gdt_desc)), "m"(*(gdt_desc + 2)), 
			  "m"(*(gdt_desc + 4)),"m"(*(gdt_desc + 5)), "m"(*(gdt_desc + 6)), 
			  "m"(*(gdt_desc + 7)));
}

static inline void set_tss_desc(unsigned char *gdt_desc, void *tss_desc) {
	set_tssldt_desc(gdt_desc, tss_desc, 0x89);
}
static inline void set_ldt_desc(unsigned char *gdt_desc, void *ldt_desc) {
	set_tssldt_desc(gdt_desc, ldt_desc, 0x82);
}

static inline void set_gate(unsigned char *gate_addr, 
							unsigned int type, unsigned dpl,
							unsigned int addr) {
	__asm__("movw %%dx, %%ax\n\t"
			"movl %%eax, %1\n\t"
			"movw %0, %%dx\n\t"
			"movl %%edx, %2"
			::"i" ((type << 8) + (dpl << 13) + 0x80000),
			 "o" (*(char *)(gate_addr)),
			 "o" (*((char *)(gate_addr) + 2)),
			 "d" ((char *)addr),
			 "a" (0x00080000));
}

inline void set_intr_gate(unsigned int vec_no,
							unsigned int type, unsigned int addr) {
	set_gate(&(_idt[vec_no * 8]), 14, 0, addr);
} 

inline void set_system_gate(unsigned int vec_no,
							unsigned int type, unsigned int addr) {
	set_gate(&(_idt[vec_no * 8]), 15, 3, addr);
}

void sched_init() {
	struct desc_struct *p = 0;

	set_tss_desc(_gdt, &(init_task.task.tss));
	set_ldt_desc(_gdt, &(init_task.task.ldt));

	p = (struct desc_struct *)(_gdt + ((FIRST_TSS_ENTRY + 2) << 3));
	int i = 1;
	for (; i < NR_TASKS; i++) {
		p->a = p->b = 0;
		p++;
		p->a = p->b = 0;
		p++;
	}
	__asm__ ("pushfl; andl 0xFFFFBFFF, (%esp); popfl");
	ltr(0);
	lldt(0);
	outb_p(0x36, 0x43);
	outb_p(LATCH & 0xFF, 0x40);
	outb(LATCH >> 16, 0x40);
	set_intr_gate(0x20, &timer_interrupt);
	outb_p(inb_p(0x21) & ~0x01, 0x21);
	set_system_gate(0x80, &system_call);
}
