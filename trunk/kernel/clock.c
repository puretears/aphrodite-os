#include "proto.h"
#include "type.h"
#include "process.h"

extern process *p_curr_proc;
extern process proc_list[NR_TASKS];
extern int int_re_enter;


void clock_handler(int irq) {
	disp_str("#");

	if (int_re_enter != 0) {
		disp_str("!");
		// When reenter clock interrupt, we don't need to
		// schedule processes.
	}	

	p_curr_proc++;
	if (p_curr_proc >= proc_list + NR_TASKS) {
		p_curr_proc = proc_list;
	}
}
