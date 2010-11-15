#include "linux/linkage.h"
#include "linux/init.h"
#include "linux/list.h"
#include "asm/spinlock.h"

static struct list_head page_address_pool;
static spinlock_t pool_lock;

/*TODO: This function is only defined if HI_MEM has been configured; that is,
 * the amount of RAM the kernel can access is larger than that mapped the 
 * kernel address space (generall 4GB.)*/
void __init page_address_init() {
	
}
