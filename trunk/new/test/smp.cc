#include <iostream>
#include "asm/spinlock.h"
#include "asm/rwlock.h"
#include <boost/thread/thread.hpp>

struct shared_obj {
	spinlock_t lock;
	unsigned int data[50];
};

struct rshared_obj {
	rwlock_t lock;
	unsigned int data[50];
};
shared_obj r;
rshared_obj r1;


extern void _spin_lock(spinlock_t *);
extern void _spin_unlock(spinlock_t *);
extern void _read_lock(rwlock_t *);
extern void _read_unlock(rwlock_t *);
extern void _write_lock(rwlock_t *);
extern void _write_unlock(rwlock_t *);

struct reader {
	void operator()() {
		std::cout<<"r1 started"<<std::endl;
		read_lock(&r1.lock);
		std::cout<<"Reader1 got the lock."<<std::endl;
		std::cout<<"r1 lock:"<<std::hex<<r1.lock.lock<<std::endl;
		for (int i = 0; i < 50; i++) {
			std::cout<<"reader1: "<<r1.data[i]<<"\n";
//			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
		read_unlock(&r1.lock);
		std::cout<<"Reader1 release the lock."<<std::endl;
	}
} reader_thread1;

struct reader2 {
	void operator()() {
		std::cout<<"r2 started"<<std::endl;
		read_lock(&r1.lock);
		std::cout<<"Reader2 got the lock."<<std::endl;
		std::cout<<"r2 lock:"<<std::hex<<r1.lock.lock<<std::endl;
		for (int i = 0; i < 50; i++) {
			std::cout<<"reader2: "<<r1.data[i]<<"\n";
//			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
		read_unlock(&r1.lock);
		std::cout<<"Reader2 release the lock."<<std::endl;
	}
} reader_thread2;

struct writer {
	void operator()() {
		write_lock(&r1.lock);
		std::cout<<"Writer got the lock."<<std::endl;
		for (int j = 0; j < 50; j++) {
			r1.data[j] = 2;
			//std::cout<<"writer:  "<<i<<"\n";
			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
		write_unlock(&r1.lock);
	}
} writer_thread;

int main() {
	spin_lock_init(&r.lock);
	rwlock_init(&r1.lock);

	for (int i = 0; i < 50; i++) {
		r.data[i] = 1;
		r1.data[i] = 3;
	}

	boost::thread reader1(reader_thread1);
	boost::thread reader2(reader_thread2);
	boost::thread writer1(writer_thread);

	reader1.join();
	reader2.join();
	writer1.join();

	return 0;
}


