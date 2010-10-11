#include <iostream>
#include "asm/spinlock.h"
#include <boost/thread/thread.hpp>

struct shared_obj {
	spinlock_t lock;
	unsigned int data[10];
};

shared_obj r;

r.data[10] = { [0 ... 9]=1 };

struct reader {
	void operator()() {
		for (int i = 0; i < 100; i++) {
			std::cout<<"reader: "<<i<<"\n";
//			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
	}
} reader_thread;

struct writer {
	void operator()() {
		for (int i = 0; i < 100; i++) {
			std::cout<<"writer:  "<<i<<"\n";
			asm volatile("pause":::"memory");
		}
		std::cout<<std::endl;
	}
} writer_thread;

int main() {
	boost::thread reader1(reader_thread);
	boost::thread writer1(writer_thread);

	reader1.join();
	writer1.join();
}


