#ifndef SYSTEM_H
#define SYSTEM_H

#define set_gate(gate_addr, dpl, type, offset) \
	__asm__ ( \
		""\
		:"=m" *((int *)gate_addr),\
		 "=m" *(1 + (int *gate_addr))\
		:\
	)

inline void set_gate(int gate_addr, int dpl, int type, int offset) {
	__asm__ __volatile__ (
		"movw %%dx, %%ax\n\t"
		"movw %2, %%dx\n\t"
		"movl %%eax, %0\n\t"
		"movl %%edx, %1"
		: "=m" *((int *)gate_addr), "=m" *(1 + (int *)gate_addr)
		: "i" (0x8000 | (dpl << 13) | (type << 8)), 
		  "d" (offset), "a" (KERNEL_CS << 16)
	)
}

inline void set_trap_gate(int vector, int offset) {
	set_gate(&idt[vector], 0, 15, offset);
}
#endif
