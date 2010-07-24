AS = nasm
CC = gcc
CXX = g++
LD = ld
vpath %.h ./include

CFLAGS = -ggdb -c -fstrength-reduce -finline-functions -nostdinc -nostdlib -nostartfiles -nodefaultlibs -fno-builtin
CXXFLAGS = -ggdb -c -nostdlib -fno-builtin -nostartfiles -nodefaultlibs -fno-exceptions -fno-rtti -fno-stack-protector

KRN_OBJS = kmain.o pic.o icxxabi.o print.o string.o 


vmlinuz: loader.o $(KRN_OBJS)
	$(LD) -T link.ld $^ -o $@ 

loader.o: boot/loader.s
	$(AS) -f elf $< -o $@

pic.o: kernel/pic.c io.h
	$(CC) $(CFLAGS) -I./include $< -o $@

kmain.o: init/kmain.cc mbinfo.h
	$(CXX) $(CXXFLAGS) -I./include $< -o $@

print.o: lib/print.c print.h
	$(CC) $(CFLAGS) -I./include $< -o $@

string.o: lib/string.c string.h
	$(CC) $(CFLAGS) -I./include $< -o $@

icxxabi.o: cxx/icxxabi.cc icxxabi.h
	$(CXX) $(CXXFLAGS) -I./include $< -o $@

.phony: clean install

install:
	mount -o loop ./bochs.new/boot.img /mnt/fd
	cp vmlinuz /mnt/fd/boot/
	umount /mnt/fd

clean:
	rm *.o
	rm vmlinuz
