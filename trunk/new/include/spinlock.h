#ifndef SPINLOCK_H
#define SPINLOCK_H

#define SPIN_LOCK_UNLOCKED { 0 }

class spinlock_t {
public:
	spinlock_t(unsigned int _lock = 0) : lock(_lock) {}
	void spin_lock_irqsave(unsigned int *flags);
	void spin_unlock_irqrestore(unsigned int flags);

private:
	// Do nothing under UP
	void spin_lock() { }
	void spin_unlock() { }
	//
	inline void local_irq_save(unsigned int *);
	inline void local_irq_restore(unsigned int);

	volatile unsigned int lock;
};

#endif
