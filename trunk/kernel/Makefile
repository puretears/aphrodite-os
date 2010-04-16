
AS = nasm
LD = ld
CC = gcc -march=i386
LDFLAGS = -s -x
CFLAGS = -Wall -fstrength-reduce -fomit-frame-pointer \
		  -finline-functions -nostdinc -I../include
OBJS = sched.o

%o:%c
	${CC} ${CFLAGS} -c $^ -o $@

kernel.o: ${OBJS}
	${LD} -r -o $@ ${OBJS}
	sync

.phony: clean

clean:
	rm *.o

