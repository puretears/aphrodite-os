#ifndef LINUX_BITOPS_H
#define LINUX_BITOPS_H

#define ADDR *((unsigned long *)addr)

static inline int constant_test_bit(int nr, const volatile unsigned long *addr) {
	return ((1 << (nr & 31)) & (addr[nr >> 5])) != 0; 
}

static inline int variable_test_bit(int nr, const volatile unsigned long *addr) {
	int oldbit;

	__asm__ __volatile__ (
		"btl %2, %1\n\t"
		"sbbl %0, %0"
		:"=r (oldbit)"
		:"m" (ADDR), "Ir" (nr));

	return oldbit;
}

#define test_bit(nr, addr) \
	(__builtin_constant_p(nr) ? \
	 constant_test_bit((nr), (addr)) : \
	 variable_test_bit((nr), (addr)))
	

#endif
