AS = nasm
CC = gcc
CXX = g++
LD = ld
vpath %.h ./include

CFLAGS = -ggdb -c -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -nostartfiles -nodefaultlibs
CXXFLAGS = -ggdb -c -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector

KRN_OBJS = kmain.o pic.o icxxabi.o 


vmlinuz.img: loader.o $(KRN_OBJS)
	$(LD) -T link.ld $^ -o $@ 

loader.o: boot/loader.s
	$(AS) -f elf $< -o $@

pic.o: kernel/pic.c io.h
	$(CC) $(CFLAGS) -I./include $< -o $@

kmain.o: kernel/kmain.cc
	$(CXX) $(CXXFLAGS) $< -o $@

icxxabi.o: cxx/icxxabi.cc icxxabi.h
	$(CXX) $(CXXFLAGS) -I./include $< -o $@

.phony: clean install

install:

clean:
	rm *.o
	rm vmlinuz.img
