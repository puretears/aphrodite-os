#ifndef CONST_H
#define CONST_H

#define GDT_SIZE 128
#define IDT_SIZE 256

/* Boolean*/
#define TRUE 1
#define FALSE 0

/* Privilege level*/
#define PRIVILEGE_KERNEL 0
#define PRIVILEGE_SERVICE 1
#define PRIVILEGE_USER 3

/* RPL*/
#define RPL_KERNEL 0
#define RPL_SERVICE 1
#define RPL_USER 3

/* 8259A interrupt control port*/
#define INT_MASTER1 0x20
#define INT_MASTER2 0x21
#define INT_SLAVE1 0xA0
#define INT_SLAVE2 0xA1


#endif 
