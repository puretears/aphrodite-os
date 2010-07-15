#include "type.h"
#include "print.h"

#define COLUMNS 80
#define ROWS    25
#define ATTRIBUTE 7
#define VEDIO_ADDR 0xB8000

// Current position
static u_int x;
static u_int y;
// Current display buffer
static volatile u_char *p_vedio;


void cls() {
	u_char *p = (u_char *)VEDIO_ADDR;

	for (int i = 0;  i < (COLUMNS * ROWS); i++) {
		*(p + i) = 0;
	}

	x = 0;
	y = 0;
}

