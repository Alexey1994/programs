@tcc 8086280B.c -w -c -nostdlib -fno-builtin -o ./bin/8086280B.elf ^
	&& cd bin ^
	&& ld -T script.ld -o 8086280B.o 8086280B.elf ^
	&& objcopy -O binary -S 8086280B.o 8086280B ^
	&& cd .. ^
	|| (pause && exit 1)