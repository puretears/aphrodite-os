#include "mbinfo.h"
#define MAGIC_NUMBER 0x2BADB002
#ifdef __cplusplus
extern "C" {
#endif

void kmain(mbinfo *pmb_info, unsigned int magic) {
	if (magic != MAGIC_NUMBER) {
		// Something wrong here.
	}

	int a = 0;
	return;
}

#ifdef __cplusplus
}
#endif
