#include <iostream>
#include <string>
#include "asm/spinlock.h"
#include "asm/rwlock.h"
#include <boost/thread/thread.hpp>


#define THREAD_INDEX template <typename f, typename p> unsigned int adapter<f, p>::_index;

typedef void (*worker_thread)(unsigned int );

extern void _spin_lock(spinlock_t *);
extern void _spin_unlock(spinlock_t *);
extern void _read_lock(rwlock_t *);
extern void _read_unlock(rwlock_t *);
extern void _write_lock(rwlock_t *);
extern void _write_unlock(rwlock_t *);

spinlock_t slock; // For display

template <typename func_t, typename para_t>
class adapter {
public:
	adapter(func_t f, para_t &p): _f(f), _p(&p) {
		_index++;
		spin_lock(&slock);
		std::cout<<"Thread "<<_index<<" started."<<std::endl;
		spin_unlock(&slock);
	};

	void operator()() {
		_f(*_p);
	}

	static unsigned int _index;
private:
	func_t _f;
	para_t *_p; // Pointers to avoid extra copying
};

template <typename f, typename p>
unsigned int adapter<f, p>::_index = 0;

struct rshared_obj {
	rwlock_t rw_lock;
	unsigned int data[50];
};
rshared_obj r1;

void reader(unsigned int i __attribute__ ((unused))) {
	unsigned int c = 0; 
	for (int i = 0; i < 49; i++) {
		read_lock(r1.rw_lock);
		c = r1.data[i];
		read_unlock(r1.rw_lock);
	}
}

void writer(unsigned int c) {
	for (int i = 0; i < 49; i++) {
		write_lock(r1.rw_lock);
		r1.data[i] = c;
		write_unlock(r1.rw_lock);
	}
}

int main() {
	spin_lock_init(&slock);
	rwlock_init(&r1.rw_lock);

	for (int i = 0; i < 50; i++) {
		r1.data[i] = 9;
	}
	int i = 0, j = 1, m = 6;
	boost::thread thr1(adapter<worker_thread, int>(reader, i));
	boost::thread thr2(adapter<worker_thread, int>(reader, i));
	boost::thread thr3(adapter<worker_thread, int>(writer, j));
	boost::thread thr4(adapter<worker_thread, int>(writer, m));

	thr1.join();
	thr2.join();
	thr3.join();
	thr4.join();

	return 0;
}


