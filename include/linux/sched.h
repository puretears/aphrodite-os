#ifndef SCHED_H
#define SCHED_H
#include "head.h"

#define NR_TASKS 64
#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS - 1]

struct tss_struct {
	unsigned int back_link;
	unsigned int esp0;
	unsigned int ss0;
	unsigned int esp1;
	unsigned int ss1;
	unsigned int esp2;
	unsigned int ss2;
	unsigned int cr3;
	unsigned int eip;
	unsigned int eflags;
	unsigned int eax;
	unsigned int ecx;
	unsigned int edx;
	unsigned int ebx;
	unsigned int esp;
	unsigned int ebp;
	unsigned int esi;
	unsigned int edi;
	unsigned int es;
	unsigned int cs;
	unsigned int ss;
	unsigned int ds;
	unsigned int fs;
	unsigned int gs;
	unsigned int ldt_sel;
	unsigned int trace_iobitmap;
};

struct task_struct {
	int state;
	struct desc_struct ldt[3]; /* 0 - zero, 1 - cs, 2 - ds&ss */
	struct tss_struct tss;
};


#define INIT_TASK \
{ 0, \			
	{ \	
		{ 0, 0 }, \
		{ 0x9F, 0x00C0FA00 }, \
		{ 0x9F, 0x00C0F200 }, \
	}, \
	{ \ 
	
	}, \
}
#endif
