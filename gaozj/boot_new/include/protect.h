#ifndef PROTECT_H
#define PROTECT_H
typedef struct descriptor
{
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_mid;
	unsigned char attr1;
	unsigned char limit_high_arr2;
	unsigned char base_hight;
}DESCRIPTOR;

#endif
