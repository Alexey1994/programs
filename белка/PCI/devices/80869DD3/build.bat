@tcc 80869DD3.c -w -c -nostdlib -fno-builtin -o ./bin/80869DD3.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80869DD3.o 80869DD3.elf ^
	&& objcopy -O binary -S 80869DD3.o 80869DD3 ^
	&& cd .. ^
	|| (pause && exit 1)