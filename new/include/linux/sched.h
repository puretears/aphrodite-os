#ifndef SCHED_H
#define SCHED_H

#include "linux/list.h"
#include "linux/type.h"
#include "asm/thread_info.h"

// The process is either executing on a CPU or waiting to be executed
#define TASK_RUNNING			0
// The process is suspended until some condition becomes true.
#define TASK_INTERRUPTIBLE		1
#define TASK_UNINTERRUPTIBLE	2
#define TASK_STOPPED			4
#define TASK_TRACED				8
#define EXIT_ZOMBIE				16
#define EXIT_DEAD				32	// The final state. 

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

	pid_t pid;
	pid_t tgid;

	task_struct *parent;

	struct list_head tasks;

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
