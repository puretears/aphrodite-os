#!/bin/bash

if [ ! -e ./tools/vmlinuz.img ];
then
	bximage -fd -size=1.44 -q ./tools/vmlinuz.img > /dev/null
fi

dd if=Image of=./tools/vmlinuz.img bs=512 conv=notrunc

if [ ! -d bochs ];
then
	mkdir bochs
fi

if [  -e bochs/vmlinuz.img ];
then
	rm bochs/vmlinuz.img
fi

if [ ! -e bochs/30M.img ];
then
	HD_RESULT=`bximage -hd -mode=flat -size=30 -q ./bochs/30M.img`
fi


mv ./tools/vmlinuz.img  ./bochs
