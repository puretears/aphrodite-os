AS = nasm
CC = gcc
CXX = g++
LD = ld
vpath %.h include
CFLAGS = -ggdb -c -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc
CXXFLAGS = -ggdb -c -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector

KRN_OBJS = main.o pic.o

.s.o:
	$(AS) -f elf $< -o $@
.c.o:
	$(CC) $(CFLAGS) $< -o $@
.cc.o:
	$(CXX) $(CXXFLAGS) $< -o $@

vmlinuz.img: loader.o kernel.o
	$(LD) -T link.ld $^ -o $@ 

loader.o: boot/loader.s

kernel.o: $(KRN_OBJS)
	$(ld) -r -o $@ $^

pic.o: pic.c io.h

main.o: kmain.cc

.phony: clean install

install:

clean:
	rm *.o
	rm vmlinuz.img
