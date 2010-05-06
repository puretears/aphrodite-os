#include "linux/head.h"
#include "linux/sched.h"

#define PAGE_SIZE 4096
#define HZ 100
#define LATCH (1193180 / HZ)

long user_stack[PAGE_SIZE >> 2] = { 0 };

union task_union {
	struct task_struct task;
	unsigned char stack[PAGE_SIZE];
};

static union task_union init_task = { INIT_TASK, };
struct task_struct *current = &(init_task.task);

struct task_struct *task[NR_TASKS] = { &(init_task.task), }

void sched_init() {
	struct desc_struct *p = NULL;

	set_tss_desc(gdt, &(init_task.task.tss));
	set_ldt_desc(gdt, &(init_task.task.ldt));

	p = gdt + ((FIRST_TSS_ENTRY + 2) << 3);
	int i = 1;
	for (; i < NR_TASKS; i++) {
		p->a = p->b = 0;
		p++;
		p->a = p->b = 0;
		p++;
	}

	__asm__ ("pushfl; and 0xFFFFBFFF, (%esp); popfl");
	ltr(0);
	ldt(0);
	outb_p(0x36, 0x43);
	outb_p(LATCH & 0xFF, 0x40);
	outb(LATCH >> 16, 0x40);
	set_intr_gate(0x20, &timer_interrupt);
	outb_p(inb_p(0x21) & ~0x01, 0x21);
	set_system_gate(0x80, &system_call);
}
