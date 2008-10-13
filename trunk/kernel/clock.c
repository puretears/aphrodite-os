#include "proto.h"
#include "type.h"
#include "process.h"

extern process *p_curr_proc;
extern process proc_list[NR_TASKS];

void clock_handler(int irq) {
	disp_str("#");

	p_curr_proc++;
	if (p_curr_proc >= proc_list + NR_TASKS) {
		p_curr_proc = proc_list;
	}
}
