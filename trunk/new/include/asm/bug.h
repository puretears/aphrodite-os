#ifndef ASM_BUG_H
#define ASM_BUG_H

#define BUG() \
	__asm__ __volatile__ ("ud2")



		//	"ud2\n\t"		\
		//	".word %c0\n\t"	\
		//	".long %c1"		\
		//	::"i" (__LINE__), "i"(__FILE__))

#define BUG_ON(condition) \
	do { if (unlikely((condition) != 0)) BUG(); } while(0)

#endif
