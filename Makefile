AS = nasm
CC = gcc
CXX = g++
CFLAGS = -mcpu=i386 -Wall -fomit-frame-pointer
LDFLAGS = -m elf_i386 -Ttext 0 -e startup32

%.o:%.c
	$(CC) $(CFLAGS) -nostdinc -Iinclude -c $< -o $@

%.bin:%.s
	$(AS) -f bin -i ./include $< -o $@

all: Image

Image: boot/boot.bin boot/setup.bin tools/system tools/build
	dd if=/boot/boot.bin of=Image bs=512 conv=notrunc
	dd if=/boot/setup.bin of=Image bs=512 seek=1 conv=notrunc
	dd if=/boot/head.bin of Image bs=512 seek=4 conv=notrunc

tools/build: tools/build.c
	$(CC) $(CFLAGS) $< -o $@

boot/boot.bin: boot/boot.s

boot/setup.bin: boot/setup.s

boot/head.bin: boot/head.s
