AS = nasm
CC = gcc
CXX = g++
CFLAGS = -ggdb -nostdinc -I include -I mm -Wall -fomit-frame-pointer
LDFLAGS = -Ttext 0 -e startup32 -lstdc++
LD = ld

%.bin:%.asm
	$(AS) -f bin -i ./boot/ $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

all: Image

Image: boot/boot.bin boot/setup.bin tools/system tools/build
	objcopy -O binary -R .note -R .comment tools/system tools/kernel
	tools/build boot/boot.bin boot/setup.bin tools/kernel > $@
	sync
	./install.sh

tools/system: boot/head init/main.o kernel/kernel.o mm/memory.o
	$(LD) $(LDFLAGS) $^ -o $@

init/main.o: init/main.cpp

mm/memory.o: mm/memory.cpp

kernel/kernel.o:
	(cd kernel; make)

tools/build: tools/build.c
	$(CC) -Wall -fomit-frame-pointer $< -o $@

boot/boot.bin: boot/boot.asm boot/protect.inc

boot/setup.bin: boot/setup.asm boot/protect.inc boot/display.inc

boot/head: boot/head.s boot/protect.inc
	$(AS) -f elf32 -i ./boot/ $< -o $@


.phony: clean

clean:
	rm -f boot/*.bin init/*.o
	(cd kernel; make clean)
	rm tools/kernel tools/system
	rm Image
