
int find_next_zero_bit(const unsigned long *addr, int size, int offset) {
	// Adust addr to make 'offset' the first 4 bytes to scan
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
			return set + offset;
	}
}
