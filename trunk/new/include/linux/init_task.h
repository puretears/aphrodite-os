#ifndef LINUX_INIT_TASK_H
#define LINUX_INIT_TASK_H

#include <linux/list.h>
#include <asm/thread_info.h>

#define INIT_TASK(tsk) \
{ \
	.state = 0,			\
	.p_thread_info = &init_thread_info, \
	.parent = &tsk, \
	.children = LIST_HEAD_INIT(tsk.children), \
	.sibling = LIST_HEAD_INIT(tsk.sibling), \
}

#endif
