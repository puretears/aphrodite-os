
.phony: ALL, clean

ALL: boot.bin loader.bin

boot.bin: boot.asm
	nasm boot.asm -o ./debug/boot.bin

loader.bin: loader.asm
	nasm loader.asm -o ./debug/loader.bin

clean:
	rm ./debug/*.bin
