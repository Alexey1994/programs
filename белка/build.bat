@mkdir "bin/components"

@nasm "components/EBR-loader.asm" -o "bin/components/EBR-loader" ^
	&& nasm "components/FAT32-loader.asm" -o "bin/components/FAT32-loader" ^
	&& nasm "components/main-loader16.asm" -o "bin/components/main-loader16" ^
	&& tcc32 -w "-Iglobal" -nostdlib -c "components/main-loader32/main.c" -o "bin/components/main-loader32.elf" ^
	&& hexing.exe ^
	&& ld -T main_loader_script.ld -o "bin/components/main-loader32.o" "bin/components/main-loader32.elf" ^
	&& objcopy -O binary -S "bin/components/main-loader32.o" "bin/components/main-loader" ^
	&& build_module32 "components/interfaces/writer" ^
	&& build_module32 "components/interfaces/console" ^
	&& build_module32 "components/interfaces/display" ^
	&& build_module32 "components/drivers/display/VGA" ^
	&& build_module32 "components/interfaces/PCI" ^
	&& build_module32 "components/drivers/PCI/106B003F" ^
	&& "copy_files.exe" ^
	&& "run.lnk" ^
	|| pause