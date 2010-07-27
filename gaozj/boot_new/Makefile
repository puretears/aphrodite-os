AS = nasm
CC = gcc
CXX = g++
LD = ld
CFLAGS = -ggdb -c -fstrength-reduce -finline-functions -nostdinc -nostdlib -nostartfiles -nodefaultlibs -fno-builtin

all: kernel.bin

kernel.bin:loader.o kernel.o print.o string.o
	${LD} -T link.ld -o $@ $^

loader.o:./boot/loader.s
	${AS} -f elf	-o $@ $<
print.o: ./lib/print.c ./include/print.h
	$(CC) $(CFLAGS) -I./include $< -o $@

string.o: ./lib/string.c ./include/string.h
	$(CC) $(CFLAGS) -I./include $< -o $@

kernel.o:./kernel/kernel.c 
	$(CC) $(CFLAGS) -I./include $< -o $@

.phony:clean
clean:
	rm *.o
	rm *.bin
