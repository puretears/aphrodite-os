#ifndef SCHED_H
#define SCHED_H

#include "list.h"

struct thread_info {
	task_struct *task;
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
