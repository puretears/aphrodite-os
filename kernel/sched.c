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


void sched_init() {

}
