#ifndef SYSTEM_H
#define SYSTEM_H

#include "linux/head.h"
#include "asm/segment.h"
#include "linux/ptrace.h"

#define set_gate(gate_addr, dpl, type, offset) \
do { \
	__asm__ __volatile__("movw %%dx, %%ax\n\t" \
		"movw %%bx, %%dx\n\t" \
		"movl %%eax, %0\n\t" \
		"movl %%edx, %1" \
		: "=m" (*((int *)gate_addr)), "=m" (*((int *)(gate_addr) + 1)) \
		: "b" (0x8000 + (dpl << 13) + (type << 8)), "d" (offset), "a" (__KERNEL_CS << 16)); \
} while(0)


#endif
