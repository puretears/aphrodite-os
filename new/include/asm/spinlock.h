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


#endif
