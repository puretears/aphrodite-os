#include "io.h"

#define PIC1 0x20 // IO base address of PIC master.
#define PIC2 0xA0 // IO base address of PIC slave.
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

#define ICW1_ICW4	0x01
#define ICW1_SINGLE 0x02
#define ICW1_INIT	0x40

#define ICW4_X86 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUFF_SLAVE 0x08
#define ICW4_BUFF_MASTER 0x0C
#define ICW4_SFNM 0x10

/*void init_8259A(u_short m_intr, u_short s_intr) {
	// Save masks
	u_char a1 = inb(PIC1_DATA);
	u_char a2 = inb(PIC2_DATA);
	// ICW1
	outb(PIC1_COMMAND, ICW1_ICW4 | ICW1_INIT);
	io_wait();
	outb(PIC2_COMMAND, ICW1_ICW4 | ICW1_INIT);
	io_wait();
	// ICW2
	outb(PIC1_DATA, m_intr);
	io_wait();
	outb(PIC2_DATA, s_intr);
	io_wait();
	// ICW3
	outb(PIC1_DATA, 4);
	io_wait();
	outb(PIC2_DATA, 2);
	io_wait();
	// ICW4
	outb(PIC1_DATA, ICW4_X86);
	io_wait();
	outb(PIC2_DATA, ICW4_X86);
	io_wait();
	// Restore saved masks
	outb(PIC1_DATA, a1);
	outb(PIC2_DATA, a2);
}*/

