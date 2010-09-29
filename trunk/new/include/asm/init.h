#ifndef ASM_INIT_H
#define ASM_INIT_H

#define __initdata __attribute__((__section__(".data.init")))

#endif
