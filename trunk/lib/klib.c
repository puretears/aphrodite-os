
#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"

char * itoa(char *p_str, unsigned int num) {
	char *p_temp = p_str;
	char c;
	//t_bool b_flag = 0; 
	
	*p_temp++ = '0';
	*p_temp++ = 'x';
	int i = 28;
	for (; i >= 0; i -= 4) {
		c = (num >> i) & 0xF;

		if (num == 0)
			*p_temp++ = '0';
		else {
			//if ((b_flag == 1) || (c > 0)) {
			//	b_flag = 1;
				c += '0';

				if (c > '9')
					c += 7;
				*p_temp++ = c;
		//	}
		}
	}
	*p_temp = 0;

	return p_str;
}

void disp_int(unsigned int _num) {
	char disp_buff[16];
	itoa(disp_buff, _num);
	disp_str(disp_buff);	
}
