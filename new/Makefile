AS = nasm
CC = gcc
CXX = g++
LD = ld
vpath %.h ./include

CFLAGS = -ggdb -c -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -nostdlib -nostartfiles -nodefaultlibs
CXXFLAGS = -ggdb -c -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector

KRN_OBJS = kmain.o pic.o icxxabi.o 


vmlinuz: loader.o $(KRN_OBJS)
	$(LD) -T link.ld $^ -o $@ 

loader.o: boot/loader.s
	$(AS) -f elf $< -o $@

pic.o: kernel/pic.c io.h
	$(CC) $(CFLAGS) -I./include $< -o $@

kmain.o: init/kmain.cc mbinfo.h
	$(CXX) $(CXXFLAGS) $< -o $@

icxxabi.o: cxx/icxxabi.cc icxxabi.h
	$(CXX) $(CXXFLAGS) -I./include $< -o $@

.phony: clean install

install:
	sudo mount -o loop ./bochs.new/boot.img /mnt/fd
	sudo cp vmlinuz /mnt/fd/boot/
	sudo umount /mnt/fd

clean:
	rm *.o
	rm vmlinuz
