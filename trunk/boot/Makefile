
.phony: ALL, clean

ALL: kernel.bin boot.bin loader.bin

boot.bin: boot.asm fat12header.inc loader.inc protect.inc
	nasm boot.asm -o ./debug/boot.bin

loader.bin: loader.asm fat12header.inc loader.inc protect.inc
	nasm loader.asm -o ./debug/loader.bin

kernel.bin: kernel.asm
	nasm kernel.asm -o ./debug/kernel.bin
clean:
	rm ./debug/*.bin
