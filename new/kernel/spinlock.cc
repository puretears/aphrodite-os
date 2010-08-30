#include "spinlock.h"

inline void spinlock_t::local_irq_save(unsigned int *flags) {
	__asm__ __volatile__ ("pushf\n\t"
		"popl %0\n\t"
		"cli" :"=g"(*flags) :/*No input*/);
}

inline void spinlock_t::local_irq_restore(unsigned int flags) {
	__asm__ __volatile__ ("pushl %0\n\t"
		"popfl\n\t" : :"g"(flags));
}

void spinlock_t::spin_lock_irqsave(unsigned int *flags) {
	local_irq_save(flags);
	spin_lock();
}

void spinlock_t::spin_unlock_irqrestore(unsigned int flags) {
	spin_unlock();
	local_irq_restore(flags);
}
