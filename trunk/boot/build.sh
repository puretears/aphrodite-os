#!/bin/bash

if [ ! -d "./debug" ]; then
	mkdir "./debug"
fi

make

cd ./debug

if [ ! -f "a.img" ]; then
	bximage -q -fd -size=1.44 a.img
fi

dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
sudo mount -o loop a.img /mnt/floppy/
sudo cp loader.bin /mnt/floppy -v
sudo cp kernel.bin /mnt/floppy -v
sudo umount /mnt/floppy

