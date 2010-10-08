#include <asm/spinlock.h>

#ifndef CONFIG_PREEMPT

void _spin_lock(spinlock_t *lock) {
	preempt_disable();
	_raw_spin_lock(lock);
}

#else

#define BUILD_LOCK_OPS(op, locktype)						\
void _##op##_lock(locktype##_t *lock) {						\
	preempt_disable();										\
	for(;;) {												\
		if (likely(_raw_##op##_trylock(lock)))				\
		break;												\
		preempt_enable();									\
		if (!(lock)->break_lock)							\
			(lock)->break_lock = 1;							\
		while(!op##_can_lock(lock) && (lock)->break_lock)	\
			cpu_relax();									\
		preempt_disable();									\
	}														\
	(lock)->break_lock = 0;									\
}

BUILD_LOCK_OPS(spin, spinlock);
#endif
