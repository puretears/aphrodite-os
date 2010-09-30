#ifndef SYSTEM_H
#define SYSTEM_H
#include "head.h"
#include "segment.h"
#include "ptrace.h"
//IDT 格式
#define set_gate(gate_addr, type, dpl, offset)\
do { \
	int __d0, __d1; \
	__asm__ __volatile__("movw %%dx, %%ax\n\t"\
				"movw %4, %%dx\n\t" \
				"movl %%eax, %0\n\t" \
				"movl %%edx, %1\n\t" \
				:"=m"(*gate_addr), "=m"(*(gate_add + 1)), "=&a" (__d0), "=&d"(__d1)\
				:"i"((short)(0x8000 + (dpl <<13) + (type << 8))), "3" (offset), "2" (KERNEL_CS << 16)); \
} while(0)

#endif
