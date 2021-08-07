@nasm ./src/main16.asm -o ./bin/kernel16 ^
	&& tcc src/main.c -w -c -nostdlib -fno-builtin -o ./bin/kernel.elf ^
	&& hexing.c ^
	&& "bin/ld" -T ./bin/script.ld -o ./bin/kernel.o ./bin/kernel.elf ^
	&& "bin/objcopy" -O binary -S ./bin/kernel.o ./bin/kernel ^
	&& write-kernel.c ^
	&& "../VM/run.lnk" ^
	|| pause