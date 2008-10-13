#Makefile for APHRODITE

#It must be the same as KERNEL_ENTRYPOINT_PHY_ADDR in load.inc
KERNEL_ENTRYPOINT = 0x30400
KERNEL_ENTRYOFFSET = 0x400

#Flags
ASM = nasm
DASM = ndisasm
CC = gcc
LD = ld
ASM_BOOT_FLAGS = -I boot/include/
ASM_KERNEL_FLAGS = -I include/ -f elf
CFLAGS = -I include/ -c -fno-builtin -fno-stack-protector
LDFLAGS = -s -Ttext $(KERNEL_ENTRYPOINT)
DASMFLAGS = -u -o $(KERNEL_ENTRYPOINT) -e $(KERNEL_ENTRYOFFSET)

#
APHRODITE_BOOT = boot/boot.bin boot/loader.bin
APHRODITE_KERNEL = kernel.bin
OBJS = kernel/kernel.o kernel/start.o kernel/i8259.o kernel/global.o kernel/main.o kernel/protect.o\
		kernel/clock.o	kernel/klib.o lib/klib.o lib/klibc.o lib/string.o
DASM_OUTPUT = kernel.bin.asm

#All phony target
.PHONY: everything final image clean realclean disasm all buildimg

everything : $(APHRODITE_BOOT) $(APHRODITE_KERNEL)

all: realclean everything

final: all clean

image: final buildimg

clean:
	rm -f $(OBJS)

realclean:
	rm -f $(OBJS) $(APHRODITE_BOOT) $(APHRODITE_KERNEL)

disasm:
	$(DASM) $(DASMFLAGS) $(APHRODITE_KERNEL) > $(DASM_OUTPUT)

#Write "boot.bin" & "loader.bin" into floppy image "aphrodite.ima"
#We assume that "aphrodite.ima" already exists.
buildimg:
	mount aphrodite.ima /mnt/floppy -o loop
	cp -f boot/loader.bin /mnt/floppy/
	cp -f kernel.bin /mnt/floppy/
	umount /mnt/floppy/

boot/boot.bin: boot/boot_loader.asm boot/include/load.inc boot/include/fat12hdr.inc
	$(ASM) $(ASM_BOOT_FLAGS) -o $@ $<

boot/loader.bin: boot/loader.asm boot/include/load.inc boot/include/fat12hdr.inc \
	boot/include/protect.inc boot/include/lib32.inc
	$(ASM) $(ASM_BOOT_FLAGS) -o $@ $<

$(APHRODITE_KERNEL): $(OBJS)
	$(LD) $(LDFLAGS) -o $(APHRODITE_KERNEL) $(OBJS)

kernel/kernel.o: kernel/kernel.asm include/sconst.inc
	$(ASM) $(ASM_KERNEL_FLAGS) -o $@ $<

kernel/main.o: kernel/main.c include/proto.h include/type.h include/const.h \
  include/protect.h

kernel/start.o: kernel/start.c include/type.h include/const.h include/protect.h \
	include/proto.h include/string.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/i8259.o: kernel/i8259.c include/type.h include/const.h include/protect.h \
	include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/global.o: kernel/global.c include/type.h include/const.h include/protect.h include/proto.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/klib.o: kernel/klib.c
	$(CC) $(CFLAGS) -o $@ $<


kernel/clock.o: kernel/clock.c include/proto.h include/type.h include/process.h include/const.h
	$(CC) $(CFLAGS) -o $@ $<

kernel/protect.o: kernel/protect.c include/proto.h include/type.h \
  include/protect.h include/type.h include/const.h include/process.h
	$(CC) $(CFLAGS) -o $@ $<

lib/klibc.o:  lib/klib.c include/type.h include/const.h include/protect.h \
  include/type.h include/proto.h include/string.h
	$(CC) $(CFLAGS) -o $@ $<

lib/klib.o: lib/klib.asm
	$(ASM) $(ASM_KERNEL_FLAGS) -o $@ $<

lib/string.o: lib/string.asm
	$(ASM) $(ASM_KERNEL_FLAGS) -o $@ $<

