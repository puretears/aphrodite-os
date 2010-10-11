#include <asm/spinlock.h>
#include <asm/atomic.h>
#include <asm/rwlock.h>


#ifndef CONFIG_PREEMPT

void _spin_lock(spinlock_t *lock) {
	preempt_disable();
	_raw_spin_lock(lock);
}

void _read_lock(rwlock_t *lock) {
	preempt_disable();
	_raw_read_lock(lock);
}

void _write_lock(rwlock_t *lock) {
	preempt_disable();
	_raw_write_lock(lock);
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
BUILD_LOCK_OPS(read, rwlock);
BUILD_LOCK_OPS(write, rwlock);
#endif /* CONFIG_PREEMPT */

void preempt_enable() {

}

void preempt_disable() {

}

void _spin_unlock(spinlock_t *lock) {
	_raw_spin_unlock(lock);
	preempt_enable();
}

void _read_unlock(rwlock_t *lock) {
	_raw_read_unlock(lock);
	preempt_enable();
}

void _write_unlock(rwlock_t *lock) {
	_raw_write_unlock(lock);
	preempt_enable();
}

static inline void _raw_spin_unlock(spinlock_t *lock) {
	char oldval = 1;
	__asm__ __volatile__ (
			spin_unlock_string );
}

