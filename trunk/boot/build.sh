#!/bin/bash

if [ ! -d "./debug" ]; then
	mkdir "./debug"
	cd ./debug
	pwd
else
	echo "debug directory has already been created."
	pwd
fi


#`dd if=boot.bin of=a.img bs=512 count=1 conv=notrunc`

#sudo mount -o loop a.img /mnt/floppy/
#sudo cp loader.bin /mnt/floppy -v
#sudo umount /mnt/floppy
