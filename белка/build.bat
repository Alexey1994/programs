@set CURRENT_PATH=%~dp0
@rem @setx path "%PATH%;%CURRENT_PATH%"

@cd kernel && build.bat && cd ..  ^
	&& cd PCI && build.bat && cd ..  ^
	&& cd PCI/devices/10025046 && build.bat && cd ../../..  ^
	&& cd PCI/devices/10222000 && build.bat && cd ../../..  ^
	&& cd PCI/devices/10EC8168 && build.bat && cd ../../..  ^
	&& cd PCI/devices/12341111 && build.bat && cd ../../..  ^
	&& cd PCI/devices/13445410 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80862415 && build.bat && cd ../../..  ^
	&& cd PCI/devices/8086280B && build.bat && cd ../../..  ^
	&& cd PCI/devices/80863EA0 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80867010 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80867111 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80869DD3 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80869DED && build.bat && cd ../../..  ^
	&& cd PCI/devices/80869DF0 && build.bat && cd ../../..  ^
	&& cd PCI/devices/80869DF5 && build.bat && cd ../../..  ^
	&& write-kernel.c ^
	&& "VM/run.lnk" ^
	|| (pause && exit 1)