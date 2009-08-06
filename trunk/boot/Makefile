
.phony: ALL

ALL: boot.bin loader.bin

boot.bin: boot.asm
	nasm boot.asm -o boot.bin

loader.bin: loader.asm
	nasm loader.asm -o loader.bin
