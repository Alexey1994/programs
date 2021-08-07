@tcc 13445410.c -w -c -nostdlib -fno-builtin -o ./bin/13445410.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 13445410.o 13445410.elf ^
	&& objcopy -O binary -S 13445410.o 13445410 ^
	&& cd .. ^
	|| (pause && exit 1)