include ../Makefile

vpath %.inc ../
kernel.bin:kernel.asm display.inc
	$(AS) $^ -o $@ 

.PHONY: clean
clean:
	rm kernel.bin
