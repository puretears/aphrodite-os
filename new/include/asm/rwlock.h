#ifndef ASM_RWLOCK_H
#define ASM_RWLOCK_H

#define __build_read_lock_ptr(rw, helper)	\
	asm volatile(LOCK "subl $1, (%0)\n\t"	\
			     "jns 1f\n\t"				\
				 "call " helper "\n\t"		\
				 "1:\n"						\
				 ::"a"(rw) :"memory")

#define __build_read_lock_const(rw, helper)	\
	asm volatile(LOCK "subl $1, %0\n\t"		\
				 "jns 1f\n\t"				\
				 "pushl %%eax\n\t"			\
				 "leal %0, %%eax\n\t"		\
				 "call " helper "\n\t"		\
			     "popl %%eax\n\t"			\
				 "1:\n"						\
				 :"=m" (*(volatile int *)rw): :"memory")


#define __build_read_lock(rw, helper) do {	\
	if (__builtin_constant_p(rw))			\
		__build_read_lock_const(rw, helper) \
	else									\
		__build_read_lock_ptr(rw, helper)	\
} while(0)

#endif
