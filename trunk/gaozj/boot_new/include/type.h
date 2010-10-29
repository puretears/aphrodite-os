#ifndef TYPE_H
#define TYPE_H

#define NULL 0
typedef unsigned char  u_char;
typedef unsigned short u_short;
typedef unsigned int   u_int;
typedef unsigned long u_int64;

typedef long int64;
//#define fastcall __attribute__((regparm(3)))
#define asmlinkage CPP_ASMLINKAGE __attribute__((regparm(0))) 

#endif

