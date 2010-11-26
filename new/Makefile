AS = as
CC = gcc
CXX = g++
LD = ld
vpath %.h ./include

CFLAGS = -DDEBUG -ggdb -Iinclude -c -fstrength-reduce -finline-functions -nostdinc -nostdlib \
		 -nostartfiles -nodefaultlibs -fno-builtin

CXXFLAGS = -DDEBUG -ggdb -c -Iinclude -nostdlib -fno-builtin -nostartfiles -nodefaultlibs \
		   -fno-exceptions -fno-rtti -fno-stack-protector

KRN_OBJS = boot/load.o boot/head.o init/kmain.o cxx/icxxabi.o lib/bitops.o lib/print.o \
		   lib/string.o mm/highmem.o mm/memory.o mm/bootmem.o mm/page_alloc.o mm/init.o\
		   kernel/pic.o kernel/traps.o kernel/sys_call.o kernel/irq.o kernel/spinlock.o \
		   kernel/setup.o kernel/pid.o kernel/semaphore.o kernel/init_task.o

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@
%.o:%.cc
	$(CXX) $(CXXFLAGS) $< -o $@
#%.o:%.S
#	$(AS) -Iinclude $< -o $@
%.o:%.asm
	nasm -f elf $< -o $@
%.o:%.s
	$(AS) -Iinclude $< -o $@
%.s:%.S
	$(CC) -E -D__ASSEMBLY__ -Iinclude $< -o $@

all: vmlinuz.lds vmlinuz

vmlinuz: $(KRN_OBJS)
	$(LD) -T vmlinuz.lds  $^ -o $@ 


vmlinuz.lds.1: vmlinuz.lds.S
	$(CC) -E -D__ASSEMBLY__ -Iinclude $< -o $@

vmlinuz.lds: vmlinuz.lds.1
	sed '/#/'d $< > $@
#-------------------------------------------------------------------------------
# File dependencies
#
# Boot
boot/head.s: boot/head.S
boot/load.s: boot/load.S
boot/head.o: boot/head.s
boot/load.o: boot/load.s

# Init modules
init/kmain.o: init/kmain.cc include/linux/mbinfo.h include/linux/type.h \
	 include/linux/print.h include/linux/type.h include/asm/page.h

# Kernel modules
kernel/sys_call.o: kernel/sys_call.asm

kernel/init_task.o: kernel/init_task.c include/linux/sched.h \
	 include/linux/list.h include/linux/type.h include/asm/thread_info.h \
	 include/linux/init_task.h

kernel/irq.o: kernel/irq.c include/linux/system.h include/linux/head.h \
		 include/linux/type.h include/asm/processor.h include/asm/segment.h \
		  include/linux/ptrace.h include/linux/type.h include/linux/irq.h \
		   include/asm/spinlock.h include/asm/rwlock.h include/asm/atomic.h \
		    include/linux/io.h

kernel/pic.o: kernel/pic.c include/linux/io.h include/linux/type.h

kernel/pid.o: kernel/pid.c include/linux/thread.h include/linux/list.h \
	 include/linux/type.h include/linux/pid.h include/asm/page.h

kernel/sched.o: kernel/sched.c include/linux/sched.h include/linux/list.h \
		 include/linux/type.h include/asm/thread_info.h

kernel/semaphore.o: kernel/semaphore.c include/asm/atomic.h include/asm/rwlock.h

kernel/setup.o: kernel/setup.c include/asm/e820.h include/linux/type.h \
	 include/asm/setup.h include/asm/page.h include/linux/init.h \
	  include/asm/pgtable.h

kernel/spinlock.o: kernel/spinlock.c include/asm/spinlock.h include/asm/rwlock.h \
		 include/asm/atomic.h

kernel/traps.o: kernel/traps.c include/linux/type.h include/linux/ptrace.h \
		 include/linux/type.h include/linux/print.h include/linux/system.h \
		  include/linux/head.h include/asm/processor.h include/asm/segment.h

# Memory modules
mm/memory.o: mm/memory.c include/linux/page.h include/linux/mm.h \
	 include/linux/type.h include/linux/head.h 

mm/bootmem.o: mm/bootmem.c include/asm/page.h include/linux/init.h \
	 include/linux/mmzone.h include/linux/bootmem.h include/linux/string.h \
	  include/linux/type.h include/asm/bitops.h include/asm/atomic.h \
	  include/linux/compiler.h include/linux/init.h

mm/page_alloc.o: mm/page_alloc.c include/linux/mmzone.h \
	 include/linux/bootmem.h

mm/init.o: mm/init.c

mm/highmem.o: mm/highmem.c include/linux/linkage.h include/linux/list.h \
	 include/linux/type.h include/asm/spinlock.h include/asm/rwlock.h \
	  include/asm/atomic.h

# Libs
lib/print.o: lib/print.c include/linux/type.h include/linux/print.h \
	 include/linux/type.h include/linux/string.h

lib/string.o: lib/string.c include/linux/string.h include/linux/type.h

lib/bitops.o: lib/bitops.c

# CXX runtimes
cxx/icxxabi.o: cxx/icxxabi.cc include/icxxabi.h



.phony: clean install

install:
	mount -o loop ./bochs.debug/30M.img /mnt/
	cp vmlinuz /mnt/boot/
	umount /mnt/

clean:
	rm boot/*.o kernel/*.o lib/*.o init/*.o mm/*.o
	rm boot/*.s
	rm vmlinuz
	rm vmlinuz.lds
	rm vmlinuz.lds.1
