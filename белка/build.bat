@set CURRENT_PATH=%~dp0
@rem @setx path "%PATH%;%CURRENT_PATH%"

@mkdir bin

@nasm kernel/kernel16.asm -o ./bin/kernel16 ^
	&& tcc kernel/kernel32.c -Iinclude -w -c -nostdlib -fno-builtin -o ./bin/kernel32.elf ^
	&& hexing.c ^
	&& ld -T kernel_script.ld -o ./bin/kernel32.o ./bin/kernel32.elf ^
	&& objcopy -O binary -S ./bin/kernel32.o ./bin/kernel ^
	&& build_module "interfaces\console" ^
	&& build_module "interfaces\video" ^
	&& build_module "interfaces\PCI" ^
	&& build_module "PCI\10025046" ^
	&& build_module "PCI\10222000"  ^
	&& build_module "PCI\106B003F"  ^
	&& build_module "PCI\10EC8168"  ^
	&& build_module "PCI\12341111"  ^
	&& build_module "PCI\13445410"  ^
	&& build_module "PCI\80861E31"  ^
	&& build_module "PCI\80862415"  ^
	&& build_module "PCI\8086280B"  ^
	&& build_module "PCI\80863EA0"  ^
	&& build_module "PCI\80867010"  ^
	&& build_module "PCI\80867111"  ^
	&& build_module "PCI\80869DD3"  ^
	&& build_module "PCI\80869DED"  ^
	&& build_module "PCI\80869DF0"  ^
	&& build_module "PCI\80869DF5"  ^
	&& write-kernel.c ^
	&& "VM/run.lnk" ^
	|| pause