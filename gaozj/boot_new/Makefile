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

kernel.o: kernel/kernel.c include/multiboot.h include/print.h include/type.h
	$(CC) $(CFLAGS) -I./include $< -o $@

.phony: clean install
clean:
	rm *.o
	rm *.bin

install:
	mount -o loop ./vmlinuz.img /mnt/
	cp ../boot_new/kernel.bin /mnt/
	sync
	umount /mnt
