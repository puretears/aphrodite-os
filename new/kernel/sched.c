#include "linux/sched.h"

#define BITMAP_SIZE ((((MAX_PRIO + 8) / 8) + sizeof(long) - 1) / sizeof(long))

struct prio_array {
	unsigned int nr_active;
	unsigned long bitmap[BITMAP_SIZE];
	struct list_head queue[MAX_PRIO];
};



