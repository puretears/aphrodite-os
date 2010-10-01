#ifndef ASM_PERCPU_H
#define ASM_PERCPU_H

#ifdef CONFIG_SMP
extern unsigned long __per_cpu_offset[NR_CPUS];

#define DEFINE_PER_CPU(type, name) \
	__attribute__((__section__(".data.percpu"))) __typeof__(type) per_cpu__##name

#define per_cpu(var, cpu) (*RELOC_HIDE(&per_cpu__#var, __per_cpu_offset[cpu]))

#define __get_cpu_var(var) per_cpu(var, smp_processor_id())
#else // No SMP support

#define DEFINE_PER_CPU(type, name) \
	__typeof__(type) per_cpu__##name

#define per_cpu(var, cpu) (* ((void)cpu, &per_cpu__##var))

#define __get_cpu_var(var) per_cpu__##var
#endif // End #ifdef CONFIG_SMP

#endif
