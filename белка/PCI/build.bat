@tcc PCI.c -w -c -nostdlib -fno-builtin -o ./bin/PCI.elf ^
	&& cd bin ^
	&& ld -T script.ld -o PCI.o PCI.elf ^
	&& objcopy -O binary -S PCI.o PCI ^
	&& cd .. ^
	|| (pause && exit 1)