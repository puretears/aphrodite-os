all:kernel.bin

kernel.bin:loader.o kernel.o
	ld -T link.ld -o $@ loader.o kernel.o

loader.o:loader.s
	nasm -f elf -o $@ loader.s

kernel.o:kernel.c
	gcc -o kernel.o -c kernel.c -ggdb -Wall -Wextra -Werror -nostdlib -nostartfiles -nodefaultlibs

.phony: clean
clean:
	rm *.o
	rm *.bin
