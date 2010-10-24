#ifndef ASM_E820_H
#define ASM_E820_H

#define E820MAX 128

#define E820_RAM 1
#define E820_RESERVED 2

#ifndef __ASSEMBLY__

struct e820map {
	int nr_map;
	struct e820entry {
		u_int64 addr;
		u_int64 size;
		unsigned int type;
	} map[E820MAX];
};

#endif

#endif
