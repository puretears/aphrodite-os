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

declare -i i
i=54
touch val.tmp
if [ ! -e bochs/30M.img ];
then
	HD_RESULT=`bximage -hd -mode=flat -size=30 -q ./bochs/30M.img`
	while [ ${i} -ne 55 ]
	do
		echo ${HD_RESULT} | awk -F" " "{print $$i}" >> val.tmp
		i=$((i+1))
	done
	cat val.tmp
	echo ${HD_RESULT_FL}
fi

rm val.tmp

mv ./tools/vmlinuz.img  ./bochs
