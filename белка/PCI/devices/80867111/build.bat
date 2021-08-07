@tcc 80867111.c -w -c -nostdlib -fno-builtin -o ./bin/80867111.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80867111.o 80867111.elf ^
	&& objcopy -O binary -S 80867111.o 80867111 ^
	&& cd .. ^
	|| (pause && exit 1)