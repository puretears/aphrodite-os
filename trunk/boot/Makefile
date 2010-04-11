include ../Makefile

OBJECT=boot.bin setup.bin
vpath %.inc ../
all:$(OBJECT)

boot.bin: boot.asm display.inc
	$(AS) $< -o $@

setup.bin: setup.asm display.inc
	$(AS) $< -o $@

.PHONY: clean

clean:
	rm *.bin
