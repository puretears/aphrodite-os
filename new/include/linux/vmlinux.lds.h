#ifndef LINUX_VMLINUX_LDS_H
#define LINUX_VMLINUX_LDS_H

#define SCHED_TEXT			\
	__sched_text_start = .;	\
	*(.sched.text)			\
	__sched_text.end = .;	\

#endif
