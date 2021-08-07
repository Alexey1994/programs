@tcc 80869DED.c -w -c -nostdlib -fno-builtin -o ./bin/80869DED.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80869DED.o 80869DED.elf ^
	&& objcopy -O binary -S 80869DED.o 80869DED ^
	&& cd .. ^
	|| (pause && exit 1)