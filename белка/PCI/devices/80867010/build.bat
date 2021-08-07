@tcc 80867010.c -w -c -nostdlib -fno-builtin -o ./bin/80867010.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80867010.o 80867010.elf ^
	&& objcopy -O binary -S 80867010.o 80867010 ^
	&& cd .. ^
	|| (pause && exit 1)