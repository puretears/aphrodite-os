AS = nasm
CC = gcc
CXX = g++
LD = ld
CFLAGS = -ggdb -c -fstrength-reduce -finline-functions -nostdinc -nostdlib -nostartfiles -nodefaultlibs -fno-builtin

vpath %.h include/
vpath %.c ./kernel:lib/:mm/

c_object = kernel.o print.o string.o memory.o traps.o sys_call.o

kernel.bin:loader.o $(c_object)
	${LD} -T link.ld -o $@ $^

sys_call.o: kernel/sys_call.s
	$(AS) -f elf boot/loader.s  -o $@

loader.o: boot/loader.s 
	$(AS) -f elf boot/loader.s  -o $@

$(c_object): %.o: %.c
	$(CC) $(CFLAGS) -I./include $< -o $@
kernel.o: kernel/kernel.c include/multiboot.h include/print.h include/type.h include/mm.h
print.o: lib/print.c include/type.h include/print.h include/type.h include/string.h
string.o: lib/string.c include/string.h include/type.h include/type.h
memory.o: mm/memory.c include/type.h include/mm.h include/type.h include/page.h include/head.h
traps.o: kernel/traps.c include/ptrace.h include/type.h include/type.h include/system.h include/head.h
#-------------------------------------------------------------------
.phony: clean install all
clean:
	rm *.o
	rm *.bin

install:
	mount -o loop ./vmlinuz.img /mnt/
	cp ../boot_new/kernel.bin /mnt/
	sync
	umount /mnt
