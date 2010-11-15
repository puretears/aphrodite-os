
OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")


ENTRY(startup_32)

SECTIONS
{
 . = 0xC0000000 + 0x100000;

 _text = .;

 .text : AT(ADDR(.text) - 0xC0000000){
  *(.text)
  __sched_text_start = .; *(.sched.text) __sched_text.end = .;
  *(.fixup)
  *(.gnu.warning)
 } = 0x9090
 _etext = .;

 . = ALIGN(16);


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

 . = ALIGN(4096);
 .data_page_aligned : { *(.data.idt) }
 _edata = .;

 . = ALIGN(8192);
 .data.init_task : { *(.data.init_task) }


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


 . = ALIGN(32);
 __per_cpu_start = .;
 .data.percpu : { *(.data.percpu) }
 __per_cpu_end = .;

 __bss_start = .;
 .bss : {
  *(.bss.page_aligned)
  *(.bss)
 }
 . = ALIGN(4);
 __bss_stop = .;

 __end = .;


 . = ALIGN(4096);
 pg0 = .;

 /DISCARD/ : {
  *(.exitcall.exit)
 }
}
