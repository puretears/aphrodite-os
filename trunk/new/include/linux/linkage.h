#ifndef LINKAGE_H
#define LINKAGE_H

#ifdef __cplusplus
#define CPP_ASMLINKAGE extern "C"
#else
#define CPP_ASMLINKAGE
#endif

#ifndef asmlinkage
#define asmlinkage CPP_ASMLINKAGE
#endif

#ifndef __ALIGN
#define __ALIGN .align 4, 0x90
#endif

//#ifdef __ASSEMBLY__
#define ALIGN __ALIGN
#define ENTRY(name) \
	.global name; \
	ALIGN; \
	name:

//#endif

#endif
