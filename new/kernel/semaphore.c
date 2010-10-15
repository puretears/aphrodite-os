#include "asm/atomic.h"
#include "asm/rwlock.h"

asm (
	//".section .sched.text\n"		
	".align 4\n"
	".global __write_lock_failed\n"
	"__write_lock_failed:\n\t"
		LOCK "addl $" RW_LOCK_BIAS_STR ",(%eax)\n\t"
	"1:\n\t"
		"pause\n\t"	
		"cmpl $" RW_LOCK_BIAS_STR ",(%eax)\n\t"
		"jne 1b\n\t"
		LOCK "subl $" RW_LOCK_BIAS_STR ", (%eax)\n\t"
		"jnz __write_lock_failed\n\t"
		"ret"
);

asm (
	//".section .sched.text\n"		
	".align 4\n"
	".global __read_lock_failed\n"
	"__read_lock_failed:\n\t"
		LOCK "incl (%eax)\n"
	"1:\n\t"
		"pause\n\t"
		"cmpl $1, (%eax)\n\t"
		"js 1b\n\t"
		LOCK "decl (%eax)\n\t"
		"js __read_lock_failed\n\t"
		"ret"
);
