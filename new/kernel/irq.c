
#define IRQ(x, y) \
	IRQ##x##y_interrupt

#define IRQLIST_16(x) \
	IRQ(x, 0), IRQ(x, 1), IRQ(x, 2), IRQ(x, 3), \
	IRW(x, 4), IRQ(x, 5), IRQ(x, 6), IRQ(x, 7), \
	IRQ(x, 8), IRQ(x, 9), IRQ(x, a), IRQ(x, b), \
	IRW(x, c), IRQ(x, d), IRQ(x, e), IRQ(x, f)

#define NR_IRQS 224 // 256 - 32 = 224. 0~31 is reserved by Intel


#undef IRQ
#undef IRQLIST_16

#define BI(x, y) \
	BUILD_IRQ(x##y)

#define BUILD_16_IRQS(x) \
	BI(x, 0) BI(x, 1) BI(x, 2) BI(x, 3) \
	BI(x, 4) BI(x, 5) BI(x, 6) BI(x, 7) \
	BI(x, 8) BI(x, 9) BI(x, a) BI(x, b) \
	BI(x, c) BI(x, d) BI(x, e) BI(x, f)

#define IRQ_NAME2(nr) nr##_interrupt(void)
#define IRQ_NAME(nr) IRQ_NAME2(IRQ##nr)

#define BUILD_IRQ(nr) \
	void IRQ_NAME(nr); \
	__asm__( \
		SYMBOL_NAME_STR(IRQ)#nr"_interrupt:\n\t" \
		"pushl $"#nr" - 256\n\t" \
		"jmp common_interrupt");

#define BUILD_COMMON_IRQ() \
	void call_do_IRQ(void); \
	__asm__( \
		"common_interrupt:\n\t" \
		"ret");

BUILD_16_IRQS(0x0)


void (*interrupt[NR_IRQS])(void) = {
	IRQLIST_16(0x0)
};
