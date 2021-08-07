@nasm src/kernel16.asm -o ./bin/kernel16 ^
	&& tcc src/kernel32.c -w -c -nostdlib -fno-builtin -o ./bin/kernel32.elf ^
	&& hexing.c ^
	&& cd bin ^
	&& ld -T script.ld -o kernel32.o kernel32.elf ^
	&& objcopy -O binary -S kernel32.o kernel ^
	&& cd .. ^
	|| (pause && exit 1)