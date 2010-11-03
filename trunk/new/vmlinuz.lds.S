#include "linux/vmlinux.lds.h"
#include "asm/thread_info.h"
#include "asm/page.h"

OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_FORMAT(i386)

ENTRY(startup_32)

SECTIONS
{
	. = __PAGE_OFFSET + 0x100000;		
	/* Read only */
	_text = .;

	.text : {
		*(.text)
		SCHED_TEXT
		*(.fixup)
		*(.gnu.warning)
	} = 0x9090
	_etext = .;

	. = ALIGN(16);

	/* Writable */
	.data : {
		__CTOR_LIST__ = .;
		LONG((__CTOR_LIST__ - __CTOR_END__) / 4 - 2)
		*(.ctor)
		LONG(0)
		__CTOR_END__ = .;

		__DTOR_LIST__ = .;
		LONG((__DTOR_LIST__ - __DTOR_END__) / 4 - 2)
		*(.dtor)
		LONG(0)
		__DTOR_END__ = .;

		*(.data)
		CONSTRUCTORS
	}

	. = ALIGN(4096)
	.data_page_aligned : { *(.data.idt) }
	_edata = .; /*End of data section*/

	. = ALIGN(THREAD_SIZE) /* Init task*/
	.data.init_task : { *(.data.init_task) }

	/* Will be freed after init */
	__init_begin = .;
	.init.text : {
		_sinittext = .;
		*(.init.text)
		_einittext = .;
	}
	.init.data : {
		*(.init.data)
	} 
	. = ALIGN(16);
	__setup_start = .;
	.init.setup : {
		*(.init.setup)
	}
	__setup_end = .;

	. = ALIGN(4096);
	__init_end = .;
	/* Freed after init ends here*/

	. = ALIGN(32);
	__per_cpu_start = .;
	.data.percpu: { *(.data.percpu) }
	__per_cpu_end = .;

	__bss_start = .; /*BSS*/
	.bss : {
		*(.bss.page_aligned)
		*(.bss)
	}
	. = ALIGNl(4);
	_bss_stop = .;

	_end = .;

	/* This is where the kernel creates the early boot page tables*/
	. = ALIGN(4096);
	pg0 = .;

	/DISCARD/ : {
		*(.exitcall.exit)
	}
}
