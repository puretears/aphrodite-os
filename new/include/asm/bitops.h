#ifndef ASM_BITOPS_H
#define ASM_BITOPS_H

#include "asm/atomic.h"

static inline int test_and_clear_bit(int nr, volatile unsigned long *addr) {
	int oldbit;

	__asm__ __volatile__ (LOCK
			"btrl %2, %1\n\t"
			"sbbl %0, %0"
			:"=r" (oldbit), "=m" (addr)
			:"Ir" (nr)
			:"memory");

	return oldbit;
}

static inline int test_and_set_bit(int nr, volatile unsigned long *addr) {
	int oldbit;

	__asm__ __volatile__ (LOCK
			"btsl %2, %1\n\t"
			"sbb %0, %0"
			:"=r" (oldbit), "=m" (*(volatile *)addr)
			:"Ir" (nr) : "memory");

	return oldbit;
}

#endif
