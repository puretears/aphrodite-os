#ifndef SCHED_H
#define SCHED_H

#include "list.h"
#include "asm/thread_info.h"

union thread_union {
	struct thread_info thread_inf;
	unsigned long stack[THREAD_SIZE / sizeof(long)];
};

// CPU specific
struct thread_struct {
	unsigned long esp0;
};


struct task_struct {
	unsigned state;
	thread_info *p_thread_info;

	task_struct *parent;

	list_head children;
	list_head sibling;

	thread_struct thread;

	unsigned int pid;
};

static inline thread_info *current_thread_info() {
	struct thread_info *ti;
	__asm__("andl %%esp, %0" :=r(ti) :""(0xFFFFE000));
	return ti;
}

static inline struct task_truct get_current() {
	return current_thread_info()->task;
}

#define current get_current()
#endif
