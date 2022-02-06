@mkdir "bin/components"

@nasm "components/EBR-loader.asm" -o "bin/components/EBR-loader" ^
	&& nasm "components/FAT32-loader.asm" -o "bin/components/FAT32-loader" ^
	&& nasm "components/kernel16.asm" -o "bin/components/kernel16" ^
	&& tcc32 -w "-Iinclude32" -nostdlib -c "components/kernel32/main.c" -o "bin/components/kernel32.elf" ^
	&& hexing.c ^
	&& ld -T kernel_script.ld -o "bin/components/kernel32.o" "bin/components/kernel32.elf" ^
	&& objcopy -O binary -S "bin/components/kernel32.o" "bin/components/kernel" ^
	&& build_module32 "components/interfaces/writer" ^
	&& build_module32 "components/interfaces/console" ^
	&& build_module32 "components/interfaces/display" ^
	&& build_module32 "components/drivers/display/VGA" ^
	&& "copy-files.c" ^
	&& "run.lnk" ^
	|| pause