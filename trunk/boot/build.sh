#!/bin/bash

if [ ! -d "./debug" ]; then
	mkdir "./debug"
else
	echo "debug directory has already been created."
fi

make

cd ./debug
dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc
sudo mount -o loop a.img /mnt/floppy/
sudo cp loader.bin /mnt/floppy -v
sudo umount /mnt/floppy

