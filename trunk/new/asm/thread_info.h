#ifndef ASM_THREAD_INFO_H
#define ASM_THREAD_INFO_H

#define THREAD_SIZE 8192

#define init_thread_info (init_thread_union.thread_inf)
#define init_thtread_stack (init_thread_union.stack)

struct thread_info {
    task_struct *task;
	unsigned long flags;
	unsigned long addr_limit;
};

#define INIT_THREAD_INFO(tsk) \
{ \
	.task = &tsk, \
	.flags = 0, \
	.addr_limit = 0xFFFFFFFF, \
}


#endif
