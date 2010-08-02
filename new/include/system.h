#ifndef SYSTEM_H
#define SYSTEM_H

#include "head.h"
#include "segment.h"

typedef void (* handler)(struct pt_regs *, int);

inline void set_gate(char *gate_addr, int dpl, int type, handler offset) {
	__asm__ __volatile__ (
		"movw %%dx, %%ax\n\t"
		"movw %2, %%dx\n\t"
		"movl %%eax, %0\n\t"
		"movl %%edx, %1"
		: "=m" (*(int *)gate_addr), "=m" (*((int *)gate_addr + 1))
		: "i" (0x8000 | (dpl << 13) | (type << 8)), 
		  "d" (offset), "a" (KERNEL_CS << 16)
	);
}

inline void set_trap_gate(int vector, handler offset) {
	set_gate(&idt[vector], 0, 15, offset);
}
#endif
