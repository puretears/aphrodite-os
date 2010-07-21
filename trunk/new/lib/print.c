#include "type.h"
#include "print.h"

#define COLUMNS 80
#define ROWS    25
#define ATTRIBUTE 7
#define VEDIO_ADDR 0xB8000

// Current position
static u_int x_pos;
static u_int y_pos;
// Current display buffer
static volatile u_char *p_vedio;


void cls() {
	u_char *p = (u_char *)VEDIO_ADDR;
	int i = 0;

	for (;  i < (COLUMNS * ROWS); i++) {
		*(p + i) = 0;
	}

	x_pos = 0;
	y_pos = 0;
}

void itoa(u_char *buf, int base, int num) {
	u_int unum = num;
	u_char *p = buf;
	u_char *p1 = buf;
	u_char *p2 = NULL;
	int divisor = 10;

	if ((num < 0) && (base == 'd')) {
		unum = -num;	
		*p++ = '-';
		buf++;
		p1 = p;
	}
	else if (base == 'x') {
		divisor = 16;	
	}
	
	int remainder = 0;

	do {
		remainder = unum % divisor;
		*p++ = (remainder < 10) ? remainder + '0' : remainder - 10 + 'a';
	}while (unum /= divisor);

	// Reverse the buf
	p1 = buf;
	p2 = p - 1;
	*p = 0;
	char tmp;

	while (p1 < p2) {
		tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

/*int main() {
	char buf[32];
	itoa(buf, 'x', 0x12345678);
	printf("%s, %x\n", buf, 0x12345678);

	return 0;
}*/

void putchar(char c) {
	if ((c == '\n') || (c == '\r')) {
	newline:
		x_pos = 0;
		y_pos++;
		
		if (y_pos >= COLUMNS)
			y_pos = 0;
		return;
	}		

	*((u_char *)(VEDIO_ADDR + (x_pos + y_pos * COLUMNS) * 2)) = c & 0xFF;
	*((u_char *)(VEDIO_ADDR + (x_pos + y_pos * COLUMNS) * 2 + 1)) = ATTRIBUTE;
	
	x_pos++;

	if (x_pos >= COLUMNS) {
		goto newline;
	}
}

void printk(const char *format, ...) {
	char *p1;
	char buf[32];

	char **multi_char = (char **)(&format);
	multi_char++; // multi_char points the next parameter of format

	char c;
	int *pn = 0;

	while ((c = *format++) != 0) {
		if (c != '%') {
			putchar(c);
		}
		else {
			c = *format++;
			switch(c) {
				case 'd':
				case 'x':
				case 'u':
					pn = (int *)multi_char;
					multi_char++;
					itoa(buf, c, *pn);
					p1 = buf;
					goto DISP_STRING;
					break;
				case 's':
					p1 = *multi_char;
					multi_char++;

					if (!p1) {
						p1 = "NULL";
					}
					DISP_STRING:

					while (*p1 != NULL) {
						putchar(*p1++);
					}
					break;
				default:
					multi_char++;
					break;
			}
		}
	}
}
