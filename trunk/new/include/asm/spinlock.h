#ifndef ASM_SPINLOCK_H
#define ASM_SPINLOCK_H

#include <asm/rwlock.h>
#include <asm/atomic.h>

/*Spinlock Related*/
typedef struct {
	volatile unsigned int slock;
#ifdef CONFIG_PREEMPT
	unsigned int break_lock;
#endif
} spinlock_t;

#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 }
#define spin_lock_init(x) do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)

#define spin_lock(lock) _spin_lock(lock)
#define spin_unlock(lock) _spin_unlock(lock)

#define spin_is_locked(x) (*(volatile signed char *)(&(x)->slock) <= 0)
#define spin_can_lock(lock) (!spin_is_locked(lock))

#define spin_lock_string	\
	"\n1:\t"				\
	"lock ; decb %0\n\t"	\
	"jns 3f\n"				\
	"2:\t"					\
	"pause\n\t"				\
	"cmpb $0, %0\n\t"		\
	"jle 2b\n\t"			\
	"jmp 1b\n"				\
	"3:\n\t"

#define spin_unlock_string	\
	"xchgb %b0, %1"			\
	:"=q" (oldval), "=m"(lock->string)	\
	:"0" (oldval) : "memory"

static inline int _raw_spin_trylock(spinlock_t *lock) {
	char oldval;

	__asm__ __volatile__ (
			"xchgb %b0, %1"
			:"=q"(oldval), "=m"(lock->slock)
			:"0" (0) : "memory");

	return oldval > 0;
}

static inline void _raw_spin_lock(spinlock_t *lock) {
	__asm__ __volatile__ (
			spin_lock_string
			:"=m" (lock->slock)::"memory");
}

/*RWlocks Realated.*/

typedef struct {
	volatile unsigned int lock;
#ifdef CONFIG_PREEMPT
	unsigned int break_lock;
#endif
} rwlock_t;

#define RW_LOCK_UNLOCKED (rwlock_t) { RW_LOCK_BIAS }
#define rwlock_init(x) do { *(x) = RW_LOCK_UNLOCKED; } while(0)

#define read_lock(lock) _read_lock(lock)
#define write_lock(lock) _write_lock(lock)

static inline void _raw_read_lock(rwlock_t *lock) {
	__build_read_lock(lock, "__read_lock_failed");
}
static inline void _raw_write_lock(rwlock_t *lock) {
	__build_write_lock(lock, "__write_lock_failed");
}

#define read_can_lock(x) ((int)((x)->lock) > 0)

static inline int _raw_read_trylock(rwlock *lock) {
	atomic_t *count = (atomic_t *)lock;
	atomic_dec(count);

	if (atomic_read(count) >= 0)
		return 1;
	atomic_inc(count);
	return 0;
}

#define read_unlock(lock) _read_unlock(lock)
#define write_unlock(lock) _write_unlock(lock)

#define _raw_read_unlock(rw) \
	asm volatile("lock; incl %0" :"=m" ((rw)->lock) : :"memory")

#define _raw_write_unlock(rw) \
	asm volatile("lock; addl $" RW_LOCK_BIAS_STR ", %0" :"=m" ((rw)->lock): :"memory")

#endif
