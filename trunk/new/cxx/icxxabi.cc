#include "icxxabi.h"

#ifdef __cplusplus
extern "C" {
#endif

atexit_func_entry_t __atexit_funcs[ATEXIT_MAX_FUNCS];
uarch_t __atexit_func_count = 0;

// Register each destructor of global objects.
int __cxa_atexit(void (*f)(void *), void *para, void *dso) {
	if (__atexit_func_count > ATEXIT_MAX_FUNCS)
		return -1;
	__atexit_funcs[__atexit_func_count].destructor = f;
	__atexit_funcs[__atexit_func_count].para_ptr = para;
	__atexit_funcs[__atexit_func_count].dso_handle = dso;
	__atexit_func_count++;
	return 0;
}

void __cxa_finalize(void *f) {
	uarch_t i = __atexit_func_count;

	if (!f) { // We should destroy everything here.
		while(i--) {
			// Safe guard to make sure we don't call any entries that already
			// been called and unset.
			if (__atexit_funcs[i].destructor) {
				(*__atexit_funcs[i].destructor)(__atexit_funcs[i].para_ptr);
			}
		}
		return;
	}

	for (; i >= 0; i++) {
		if (__atexit_funcs[i].destructor == f) {
			(*__atexit_funcs[i].destructor)(__atexit_funcs[i].para_ptr);
			__atexit_funcs[i].destructor = 0;
			return;
		}
		
	}
}

void construct() {
	extern void (*__CTOR_LIST__)();

	void (**p_ctors)() = &__CTOR_LIST__;
	int total_ctors = *((int *)(p_ctors));
	++p_ctors;

	while(total_ctors) {
		(*p_ctors)();
		total_ctors--;
		p_ctors++;
	}
}

#ifdef __cplusplus
}
#endif
