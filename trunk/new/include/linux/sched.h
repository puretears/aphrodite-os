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

#define MAX_USER_RT_PRIO 100
#define MAX_RT_PRIO MAX_USER_RT_PRIO

#define MAX_PRIO (MAX_RT_PRIO + 40)


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
	struct thread_info *p_thread_info;

	struct list_head tasks;

	pid_t pid;
	pid_t tgid;

	struct task_struct *real_parent;
	struct task_struct *parent;

	struct list_head children;
	struct list_head sibling;

	struct task_struct *group_leader;

	struct thread_struct thread;
};

static inline struct task_struct *get_current() {
	return current_thread_info()->task;
}

#define current get_current()
#endif
