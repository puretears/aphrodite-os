#ifndef TYPE_H
#define TYPE_H

typedef unsigned int t_32;
typedef unsigned short t_16;
typedef unsigned char t_8;
typedef int t_bool;
typedef unsigned int t_port;
typedef void * t_sys_call;

//typedef void (* interrupt_handler) ();
typedef void (* t_pf_task)();
typedef void (* interrupt_handler) (int irq);


#endif

