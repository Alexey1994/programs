@tcc 80869DF5.c -w -c -nostdlib -fno-builtin -o ./bin/80869DF5.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 80869DF5.o 80869DF5.elf ^
	&& objcopy -O binary -S 80869DF5.o 80869DF5 ^
	&& cd .. ^
	|| (pause && exit 1)