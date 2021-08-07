@tcc 10025046.c -w -c -nostdlib -fno-builtin -o ./bin/10025046.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 10025046.o 10025046.elf ^
	&& objcopy -O binary -S 10025046.o 10025046 ^
	&& cd .. ^
	|| (pause && exit 1)