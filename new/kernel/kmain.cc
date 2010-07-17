#include "mbinfo.h"
#include "print.h"
#define MAGIC_NUM 0x2BADB002

void kmain(mbinfo *pmb, u_int magic_num) {
	if (magic_num != MAGIC_NUM) {
		printk("Invalid kernel image.\n");
	}

	print("Kernel load successful.\n");
	return;
}
