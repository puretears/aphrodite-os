#ifndef HEAD_H
#define HEAD_H
#include "type.h"
struct desc_struct{
	int low;
	int hight;
};
extern struct desc_struct idt[256];
extern u_int swapper_pg_dir[1024];
#endif
