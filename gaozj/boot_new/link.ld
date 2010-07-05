ENTRY (entry_addr)

SECTIONS{
	. = 0x0100000;

	.text : {
			load_addr = .;
			*(.text)
			. = ALIGN (4096);
	}

	.rodata : {
		*(.rodata)
		. = ALIGN (4096);
	}

	.data : {
		*(.data)
		. = ALIGN (4096);
	}

	.bss : {
		bss_end_addr = .;
		*(COMMON)
		*(.bss)
	}
	load_end_addr = .;
}
