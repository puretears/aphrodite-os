AS = nasm
CC = gcc
CXX = g++
CFLAGS = -march=i386 -Wall -fomit-frame-pointer
LDFLAGS = -m elf_i386 -Ttext 0 -e startup32

%.o:%.c
	$(CC) $(CFLAGS) -nostdinc -Iinclude -c $< -o $@

all: Image

Image: boot/boot.bin boot/setup.bin tools/system tools/build
	objcopy -O binary -R .note -R .comment tools/system tools/kernel
	#tools/build boot/boot.bin boot/setup.bin tools/kernel > $@

tools/system: boot/head init/main.o 
	$(LD) $(LDFLAGS) $^ -o $@

init/main.o: init/main.c

tools/build: tools/build.c
	$(CC) $(CFLAGS) $< -o $@

boot/boot.bin: boot/boot.asm boot/protect.inc
	$(AS) -f bin -i ./boot/ $< -o $@

boot/setup.bin: boot/setup.asm boot/protect.inc
	$(AS) -f bin -i ./boot/ $< -o $@

boot/head: boot/head.s boot/protect.inc
	$(AS) -f elf32 -i ./boot/ $< -o $@

.phony: clean

clean:
	rm -f Image boot/*.bin init/*.o kernel/*.bin