#ifndef ASM_SPINLOCK_H
#define ASM_SPINLOCK_H

typedef struct {
	volatile unsigned int slock;
#ifdef CONFIG_PREEMPT
	unsigned int break_lock;
#endif
} spinlock_t;

#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 }
#define spin_lock_init(x) do { *(x) = SPIN_LOCK_UNLOCKED; } while(0)

#define spin_lock(lock) _spin_lock(lock)

#define spin_is_locked(x) (*(volatile signed char *)(&(x)->slock) <= 0)
#define spin_can_lock(lock) (!spin_is_locked(lock))

#define spin_unlock(lock) _spin_unlock(lock)

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
	__asm__ __volatie__ (
			spin_lock_string
			:"=m" (lock->slock)::"memory");
}


#endif
