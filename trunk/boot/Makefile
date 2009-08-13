
.phony: ALL, clean

ALL: boot.bin loader.bin

boot.bin: boot.asm fat12header.inc
	nasm boot.asm -o ./debug/boot.bin

loader.bin: loader.asm fat12header.inc
	nasm loader.asm -o ./debug/loader.bin

clean:
	rm ./debug/*.bin
