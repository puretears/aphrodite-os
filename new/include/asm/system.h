#ifndef ASM_INCLUDE_SYSTEM_H
#define ASM_INCLUDE_SYSTEM_H

// Original impl: 
// #define mb() __asm__ __volatile__ ("lock; addl $0, 0(%%esp)":::"memory")
#define mb() __asm__ __volatile__ ("mfence")
#define rmb() __asm__ __volatile__ ("lfence")
#define wmb() __asm__ __volatile__ ("":::"memory")

#define barrier() __asm__ __volatile__ ("":::"memory")

#ifdef CONFIG_SMP
#define smp_mb() mb()
#define smp_rmb() rmb()
#define smp_wmb() wmb()
#else
#define smp_mb() barrier()
#define smp_rmb() barrier()
#define smp_wmb() barrier()
#endif

#endif
