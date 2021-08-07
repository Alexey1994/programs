@tcc 10222000.c -w -c -nostdlib -fno-builtin -o ./bin/10222000.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 10222000.o 10222000.elf ^
	&& objcopy -O binary -S 10222000.o 10222000 ^
	&& cd .. ^
	|| (pause && exit 1)