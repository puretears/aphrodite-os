#include "asm/io.h"
#include "linux/time.h"
#include "memory.h"

#define CURR_POS 0x90000
#define TOTAL_MEM 0x90004
#define DISK_PARAM 0x90080


unsigned char CMOS_READ(unsigned char addr) {
	outb_p(addr, 0x70);  
	return inb_p(0x71); 
}

#define BCD2DEC(val) ((val) = ((val) & 15) + ((val) >> 4) * 10)

static void time_init() {
	struct tm time;

	do {
		time.tm_sec = CMOS_READ(0);
		time.tm_min = CMOS_READ(2);
		time.tm_hour = CMOS_READ(4);
		time.tm_mday = CMOS_READ(7);
		time.tm_mon = CMOS_READ(8);
		time.tm_year  =CMOS_READ(9);
	} while (time.tm_sec != CMOS_READ(0));

	BCD2DEC(time.tm_sec);
	BCD2DEC(time.tm_min);
	BCD2DEC(time.tm_hour);
	BCD2DEC(time.tm_mday);
	BCD2DEC(time.tm_mon);
	BCD2DEC(time.tm_year);
}

	int main() {
		time_init();
		unsigned short curr_pos = *((unsigned short *)CURR_POS);
		//unsigned int total_mem = (*(unsigned int *)(TOTAL_MEM));
		while(1) {};
		return 0;
}
