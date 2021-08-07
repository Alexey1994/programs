@tcc 12341111.c -w -c -nostdlib -fno-builtin -o ./bin/12341111.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 12341111.o 12341111.elf ^
	&& objcopy -O binary -S 12341111.o 12341111 ^
	&& cd .. ^
	|| (pause && exit 1)