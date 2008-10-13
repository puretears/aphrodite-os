#ifndef PROTO_H
#define PROTO_H

#include "type.h"

/* klib.asm*/
void write_to_port(t_port port, t_8 value);
int read_from_port(t_port port);
void disp_str(char *p_str);
void disp_color_str(char *p_str, int color);
void disp_int(unsigned int num);

/* protect.c*/
void init_idt();
t_32 sel_to_phys(t_16 sel);

/* kernel.asm*/
void restart();

/* klib.c*/
void delay(int time);

/* main.c*/
void process0();
void process1();

/* clock.c*/
void clock_handler(int irq);

#endif
