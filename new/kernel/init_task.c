#include <linux/sched.h>
#include "linux/init_task.h"
#include <asm/thread_info.h>

struct task_struct init_task;

union thread_union init_thread_union __attribute__((__section__(".data.init_task"))) = { INIT_THREAD_INFO(init_task) };

struct task_struct init_task = INIT_TASK(init_task);
