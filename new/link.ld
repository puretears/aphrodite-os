ENTRY(loader)

SECTIONS
{
	.=0x100000
	.text : 
	{
		code = .; text = .;
		*(.text*)
		*(.gnu.linkonce.t*)
		. = ALIGN(4096);
	}
	.rodata :
	{
		*(.rodata*)
		*(.gnu.linkonce.r*)
		. = ALIGN(4096)
	}
	.data :
	{
		__CTOR_LIST__ = .;
		LONG((__CTOR_LIST__ - __CTOR_END__) / 4 - 2)
		*(.ctor)
		LONG(0)
		__CTOR_END__ = .;

		__DTOR_LIST__ = .;
		LONG((__DTOR_LIST__ - __DTOR_END__) / 4 - 2)
		*(.dtor)
		LONG(0)
		__DTOR_LIST__ = .;

		data = .;

		*(.data*)
		*(.gnu.linkonce.d*)

		. = ALIGN(4096)
	}
	.bss
	{
		bss = .;
		*(.COMMON*)
		*(.bss*)
		*(.gnu.linkonce.b*)
		. = ALIGN(4096)
	}

	end = .

	/DISCARD/:
	{
		*(.comment)
		*(.eh_frame)
	}
	
}
