AS = nasm
CC = gcc
CXX = g++
CFLAGS = -Iinclude -Wall -fomit-frame-pointer
LDFLAGS = -Ttext 0 -e startup32

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: Image

Image: boot/boot.bin boot/setup.bin tools/system tools/build
	objcopy -O binary -R .note -R .comment tools/system tools/kernel
	tools/build boot/boot.bin boot/setup.bin tools/kernel > $@
	sync

tools/system: boot/head init/main.o kernel/sched.o 
	$(LD) $(LDFLAGS) $^ -o $@

init/main.o: init/main.c

kernel/kernel.o:
	(cd kernel; make)

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
	rm -f boot/*.bin init/*.o
	(cd kernel; make clean)
	rm tools/kernel tools/system
	rm Image
