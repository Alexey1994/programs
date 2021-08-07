@tcc 80869DF0.c -w -c -nostdlib -fno-builtin -o ./bin/80869DF0.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80869DF0.o 80869DF0.elf ^
	&& objcopy -O binary -S 80869DF0.o 80869DF0 ^
	&& cd .. ^
	|| (pause && exit 1)