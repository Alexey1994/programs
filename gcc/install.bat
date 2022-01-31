@reg delete "HKCR\*\shell\Скомпилировать GCC" /f

@set GCC_PATH=%~dp0

@"%GCC_PATH%bin\dlltool.exe" --kill-at --def "%GCC_PATH%include\level1\Windows\kernel32.def" --dllname kernel32.dll --output-lib "%GCC_PATH%include\level1\Windows\kernel32.a"

@reg add "HKCR\*\shell\Скомпилировать GCC\command" /t REG_SZ /d "\"%GCC_PATH%bin\gcc.exe\" -w -nostdlib -nostartfiles -s -Os --entry=_start -Wl,--gc-sections -DDEBUG -D__WIN32__ \"-I%GCC_PATH%include\level0\" \"-I%GCC_PATH%include\level1\" \"-I%GCC_PATH%include\level2\" \"%%1\" \"%GCC_PATH%include\level1\Windows\kernel32.a\""