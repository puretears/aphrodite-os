#include "linux/thread.h"
#include "linux/list.h"
#include "linux/pid.h"
#include "asm/page.h"

static struct hlist_head *pid_hash[PIDTYPE_MAX];

#define PIDMAP_ENTRIES ((PID_MAX_LIMIT + 8 * PAGE_SIZE - 1) / PAGE_SIZE / 8)
