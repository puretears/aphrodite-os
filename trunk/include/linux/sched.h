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
		0, (PAGE_SIZE + &init_task), 0x10, 0, 0, 0, 0, pde, 0, 0, \
		0, 0, 0, 0, 0, 0, 0, 0, \
		0x17, 0x17, 0x17, 0x17, 0x17, 0x17, _LDT(0), 0x80000000 \
	}, \
}

#define FIRST_TSS_ENTRY 4
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _LDT(n) ((FIRST_TSS_ENTRY << 3) + (n << 4))
#define _TSS(n) ((FIRST_LDT_ENTRY << 3) + (n << 4))

static inline unsigned int str() {
	int tss_sel;
	__asm__("str %%ax\n\t"
		"subl %2, %%eax\n\t"
		"shrl $4, %%eax"
		:"=a(tss_sel)"
		:"a"(0), "i"(FIRST_TSS_ENTRY << 3));
	return tss_sel;
}

static inline void set_tssldt_desc(void *gdt_desc, void *desc, char type) {
	__asm__("movw $104, %2\n\t"
		"movw %%ax, %3\n\t"
		"shrw $16, %%eax\n\t"
		"movb %%al, %4\n\t"
		"movb %%bl, %5\n\t"
		"movb $0, %6\n\t"
		"movb %%ah, %7\n\t"
		::"a"(desc), "b"(type), "m"(*(gdt_desc)), "m"(*(gdt_desc + 2)), "m"(*(gdt_desc + 4)),
		"m"(*(gdt_desc + 5)), "m"(*(gdt_desc + 6)), "m"(*(gdt_desc + 7)));
}

static inline void set_tss_desc(void *gdt_desc, void *tss_desc, char type) {
	set_tss_ldt(gdt_desc, tss_desc, 0x89);
}
static inline void set_ldt_desc(void *gdt_desc, void *tss_desc, char type) {
	set_tss_ldt(gdt_desc, tss_desc, 0x82);
}
#endif
