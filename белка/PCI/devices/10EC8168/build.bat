@tcc 10EC8168.c -w -c -nostdlib -fno-builtin -o ./bin/10EC8168.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 10EC8168.o 10EC8168.elf ^
	&& objcopy -O binary -S 10EC8168.o 10EC8168 ^
	&& cd .. ^
	|| (pause && exit 1)