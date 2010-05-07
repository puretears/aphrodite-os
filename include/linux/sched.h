#ifndef SCHED_H
#define SCHED_H
#include "head.h"

#define NR_TASKS 64
#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS - 1]
#define PAGE_SIZE 4096

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



#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _LDT(n) ((FIRST_TSS_ENTRY << 3) + (n << 4))
#define _TSS(n) ((FIRST_LDT_ENTRY << 3) + (n << 4))

static inline void lldt(unsigned int n) {
	__asm__ ("lldt %%ax"::"a"(_LDT(n)));
}

static inline void ltr(unsigned int n) {
	__asm__ ("ltr %%ax"::"a"(_TSS(n)));
}

static inline unsigned int str() {
	int tss_sel;
	__asm__("str %%ax\n\t"
		"subl %2, %%eax\n\t"
		"shrl $4, %%eax"
		:"=a" (tss_sel)
		:"a"(0), "i"(FIRST_TSS_ENTRY << 3));
	return tss_sel;
}
#endif
