#ifndef PROTO_H
#define PROTO_H

#include "type.h"

/* klib.asm*/
void write_to_port(t_port port, t_8 value);
int read_from_port(t_port port);
void disp_str(char *p_str);
void disp_color_str(char *p_str, int color);
void disp_int(unsigned int num);
int disable_irq(int irq);
int enable_irq(int irq);

/* protect.c*/
void init_idt();
t_32 sel_to_phys(t_16 sel);

/* kernel.asm*/
void restart();
void sys_call(); // IDT entry for system calls

/* klib.c*/
void delay(int time);

/* main.c*/
void process0();
void process1();

/* clock.c*/
void clock_handler(int irq);

/* i8259.c*/
void put_irq_handler(int irq, interrupt_handler pf);
void spurious_irq(int irq);

/* syscall.asm */
void get_ticks();

/* process.c*/
int sys_get_ticks(); // only used for system call table definition


#endif
