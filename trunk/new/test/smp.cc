#include <iostream>
#include "asm/spinlock.h"
#include <boost/thread/thread.hpp>

struct shared_obj {
	spinlock_t lock;
	unsigned int data[50];
};

shared_obj r;

//r.data[] = { [0 ... 9]=1 };

extern void _spin_lock(spinlock_t *);
extern void _spin_unlock(spinlock_t *);
struct reader {
	void operator()() {
		spin_lock(&r.lock);
		std::cout<<"Reader got the lock."<<std::endl;
		for (int i = 0; i < 50; i++) {
			std::cout<<"reader: "<<r.data[i]<<"\n";
//			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
		spin_unlock(&r.lock);
	}
} reader_thread;

struct writer {
	void operator()() {
		spin_lock(&r.lock);
		std::cout<<"Writer got the lock."<<std::endl;
		for (int i = 0; i < 50; i++) {
			r.data[i] = 2;
			//std::cout<<"writer:  "<<i<<"\n";
			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
		spin_unlock(&r.lock);
	}
} writer_thread;

int main() {
	spin_lock_init(&r.lock);
	for (int i = 0; i < 50; i++) {
		r.data[i] = 1;
	}

	boost::thread reader1(reader_thread);
	boost::thread writer1(writer_thread);

	reader1.join();
	writer1.join();

	return 0;
}


