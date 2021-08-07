@tcc 80863EA0.c -w -c -nostdlib -fno-builtin -o ./bin/80863EA0.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80863EA0.o 80863EA0.elf ^
	&& objcopy -O binary -S 80863EA0.o 80863EA0 ^
	&& cd .. ^
	|| (pause && exit 1)