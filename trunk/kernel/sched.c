#include "linux/head.h"
#include "linux/sched.h"

#define PAGE_SIZE 4096
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
}
