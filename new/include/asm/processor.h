#ifndef ASM_PROCESSOR_H
#define ASM_PROCESSOR_H

static inline void rep_nop() {
	__asm__ __volatile__ ("pause":::"memory");
}
#define cpu_relax() rep_nop()

#endif
