#include "linux/thread.h"
#include "asm/page.h"

#define PIDMAP_ENTRIES ((PID_MAX_LIMIT + 8 * PAGE_SIZE - 1) / PAGE_SIZE / 8)
