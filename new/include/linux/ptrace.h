#ifndef PTRACE_H
#define PTRACE_H

#include "type.h"

struct pt_regs {
	int ebx;
	int ecx;
	int edx;
	int esi;
	int edi;
	int ebp;
	int eax;
	short ds, _dsu;
	short es, _esu;
	int orig_eax;
	int eip;
	int cs;
	int eflags;
	int old_esp;
	short ss, _ssu;
};

#endif
