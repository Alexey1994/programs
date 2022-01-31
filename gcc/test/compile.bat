@REM "../bin/gcc" -S -w -nostdlib -nostartfiles -s -Os -fdata-sections -ffunction-sections -e main main.c -Wl,--gc-sections kernel32.a || pause

@"../bin/gcc" -S -w -nostdlib -nostartfiles -s -Os --entry=start -Wl,--gc-sections main.c || pause