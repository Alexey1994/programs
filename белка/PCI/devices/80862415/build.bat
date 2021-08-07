@tcc 80862415.c -w -c -nostdlib -fno-builtin -o ./bin/80862415.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80862415.o 80862415.elf ^
	&& objcopy -O binary -S 80862415.o 80862415 ^
	&& cd .. ^
	|| (pause && exit 1)