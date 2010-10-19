#ifndef ASM_CURRENT_H
#define ASM_CURRENT_H

#include "asm/thread_info.h"

struct task_struct;

static inline struct task_struct *get_current() {
	return current_thread_info()->task;
}

#define current get_current()

#endif
