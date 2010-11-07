#ifndef ASM_THREAD_INFO_H
#define ASM_THREAD_INFO_H

#define THREAD_SIZE 8192

#define init_thread_info (init_thread_union.thread_inf)
#define init_thtread_stack (init_thread_union.stack)


#ifndef __ASSEMBLY__

#include <linux/sched.h>

struct thread_info {
    struct task_struct *task;
	unsigned long flags;
	unsigned long addr_limit;
};

static inline struct thread_info *current_thread_info() {
	struct thread_info *ti;
	__asm__ ("andl %%esp, %0" :"=r" (ti) :"0" (~(THREAD_SIZE - 1)));
	return ti;
}
#endif

#define INIT_THREAD_INFO(tsk) \
{ \
	.task = &tsk, \
	.flags = 0, \
	.addr_limit = 0xFFFFFFFF, \
}


#endif
