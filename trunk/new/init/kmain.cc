#ifdef __cplusplus
extern "C" {
#endif

#include "mbinfo.h"
#include "print.h"
#define MAGIC_NUM 0x2BADB002


void kmain(mbinfo *pmb, u_int magic_num) {
	if (magic_num != MAGIC_NUM) {
		printk("Invalid kernel image.\n");
	}

	cls();

	printk("Kernel load successful.\n");
	return;
}

#ifdef __cplusplus
}
#endif
