#include "linux/head.h"
#include "linux/sched.h"

#define PAGE_SIZE 4096
long user_stack[PAGE_SIZE >> 2] = { 0 };


void sched_init() {

}
