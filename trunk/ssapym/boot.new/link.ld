ENTRY (loader)

SECTIONS{
	. = 0x00100000;

	.text : {
		code = .;
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
		bss = .;
		*(COMMON)
		*(.bss)
	}
	end = .;
}
