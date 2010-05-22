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

