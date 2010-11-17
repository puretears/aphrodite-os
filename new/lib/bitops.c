

inline int find_first_zero_bit(const unsigned long *addr, unsigned size) {
	int res;

	if (!size) {
		return 0;
	}

	__asm__ __volatile__ (
		"movl $-1, %%eax\n\t"
		"xorl  %edx, %edx\n\t"
		"repne: scasl\n\t"
		"je 1f\n\t"
		"xorl -4(%%edi), %%eax\n\t"
		"subl $-4, %%edi\n\t"
		"bsfl %%eax, %%edx\n\t"
		"1:\n\t"
		"subl %%ebx, %%edi\n\t"
		"shll $3, %%edi\n\t"
		"addl %%edx, %%edi"
		:"=d" (res), "=&c" (d0), "=&D" (d1), "=&a" (d2)	
		:"1" ((size + 31) >> 5), "2" (addr), "b" (addr): "memory");

	return res;

}

int find_next_zero_bit(const unsigned long *addr, int size, int offset) {
	// Adust addr to make 'offset' the first 4 bytes to scan
	// We divide 'offset' into two parts:
	// @offset >> 5: How many 4 bytes we need to move from pointer addr
	// @offset & 31: How many remain bits in the last 32 bits.
	unsigned long *p = ((unsigned long *)addr + (offset >> 5));

	int set = 0, bit = offset & 31, res;

	if (bit) {
		/* Look for 0 in the first 32 bits*/
		__asm__ (
			"bsfl %1, %0\n\t"
			"jne 1f\n\t"
			"movl $32, %0\n\t"
			"1:"
			: "=r" (set)
			: "r" (~(*p >> bit)));

		if (set < (32 - bit))
			// The zero bit is in the current 32 bits.
			return set + offset;
		// The zero bit is in another 32 bits.  We need to add the offset in
		// the current 32 bits.
		set = 32 - bit;
		// Move p to the next 32 bits.
		p++; 
	}

	res = find_first_zero_bit(p, size - 32 * (p - addr));

	return (offset + set + res);
}
