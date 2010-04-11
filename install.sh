#!/bin/bash

if [ ! -e ./tools/vmlinuz.img ]
then
	bximage -fd -size=1.44 -q ./tools/vmlinuz.img > /dev/null
fi

cd boot
make all
cp *.bin ../tools

cd ../kernel
make
cp kernel.bin ../tools

cd ../tools
make

./build

dd if=boot.bin of=vmlinuz.img bs=512 conv=notrunc
dd if=setup.bin of=vmlinuz.img bs=512 seek=1 conv=notrunc
dd if=kernel.bin of=vmlinuz.img bs=512 seek=5 conv=notrunc

rm *.bin
cd ..

if [ ! -d bochs ]
then
	mkdir bochs
fi

if [  -e bochs/vmlinuz.img ]
then
	rm bochs/vmlinuz.img
fi

mv ./tools/vmlinuz.img ./bochs
