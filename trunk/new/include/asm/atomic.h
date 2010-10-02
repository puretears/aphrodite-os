#ifndef ASM_ATOMIC_H
#define ASM_ATOMIC_H

#ifdef CONFIG_SMP
#define LOCK "lock ;"
#else
#define LOCK ""
#endif

typedef struct { volatile int counter; } atomic_t;

#define atomic_read(v) ((v)->counter)
// Set i to v
#define atomic_set(v, i) (((v)->counter) = (i))

// Add i to v
static __inline__ void atomic_add(int i, atomic_t *v) {
	__asm__ __volatile__ (
			LOCK "addl %1, %0"
			:"=m" (v->counter)
			:"ir" (i), "m" (v->counter))
}

// Sub i to v and test result
static __inline__ int atomic_sub_and_test(int i, atomic_t *v) {
	int c;

	__asm__ __volative__ (
		LOCK "subl %2, %0\n\t"
		"sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"ir" (i), "m" (v_counter) : "memory");

	return c;
}
#endif
