@assoc .c=
@ftype runnable_c_application=
@reg delete HKCR\.c /f
@reg delete HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.c /f
@reg delete HKCR\*\shell\Скомпилировать /f
@reg delete "HKCR\*\shell\Скомпилировать DLL" /f

@set TCC_PATH=%~dp0

@ftype runnable_c_application="%TCC_PATH%tcc.exe" -w -DDEBUG -D__WIN32__ "-I%TCC_PATH%include\level0" "-I%TCC_PATH%include\level1" "-I%TCC_PATH%include\level2" ^
	"%TCC_PATH%include\level1\Windows\kernel32.def" ^
	"%TCC_PATH%include\level1\Windows\winmm.def" ^
	"%TCC_PATH%include\level1\Windows\gdi32.def" ^
	"%TCC_PATH%include\level1\Windows\user32.def" ^
	"%TCC_PATH%include\level1\Windows\opengl32.def" ^
	"%TCC_PATH%include\level1\Windows\avicap32.def" ^
	"%TCC_PATH%include\level1\Windows\ws2_32.def" ^
	"%TCC_PATH%include\level1\Windows\dxva2.def" ^
	"%TCC_PATH%include\level1\Windows\shell32.def" ^
	"%TCC_PATH%include\level1\Windows\ole32.def" ^
	"%TCC_PATH%include\level1\Windows\msvcrt.def" ^
	"%TCC_PATH%include\avformat.def" ^
	"%TCC_PATH%include\avcodec.def" ^
	"%TCC_PATH%include\avutil.def" ^
	"%TCC_PATH%include\swscale.def" ^
	-run "%%1"
@assoc .c=runnable_c_application
@reg add HKCR\*\shell\Скомпилировать\command /t REG_SZ /d "\"%TCC_PATH%tcc.exe\" -w -DDEBUG -D__WIN32__ \"-I%TCC_PATH%include\level0\" \"-I%TCC_PATH%include\level1\" \"-I%TCC_PATH%include\level2\" \"%%1\" \"%TCC_PATH%include\level1\Windows\kernel32.def\" \"%TCC_PATH%include\level1\Windows\winmm.def\" \"%TCC_PATH%include\level1\Windows\gdi32.def\" \"%TCC_PATH%include\level1\Windows\user32.def\" \"%TCC_PATH%include\level1\Windows\opengl32.def\" \"%TCC_PATH%include\level1\Windows\avicap32.def\" \"%TCC_PATH%include\level1\Windows\ws2_32.def\" \"%TCC_PATH%include\level1\Windows\dxva2.def\" \"%TCC_PATH%include\level1\Windows\shell32.def\" \"%TCC_PATH%include\level1\Windows\ole32.def\" \"%TCC_PATH%include\level1\Windows\msvcrt.def\" \"%TCC_PATH%include\avformat.def\" \"%TCC_PATH%include\avcodec.def\" \"%TCC_PATH%include\avutil.def\" \"%TCC_PATH%include\swscale.def\""
@reg add "HKCR\*\shell\Скомпилировать DLL\command" /t REG_SZ /d "\"%TCC_PATH%tcc.exe\" -w -DDEBUG -D__WIN32__ -shared \"-I%TCC_PATH%include\level0\" \"-I%TCC_PATH%include\level1\" \"-I%TCC_PATH%include\level2\" \"%%1\" \"%TCC_PATH%include\level1\Windows\kernel32.def\" \"%TCC_PATH%include\level1\Windows\winmm.def\" \"%TCC_PATH%include\level1\Windows\gdi32.def\" \"%TCC_PATH%include\level1\Windows\user32.def\" \"%TCC_PATH%include\level1\Windows\opengl32.def\" \"%TCC_PATH%include\level1\Windows\avicap32.def\" \"%TCC_PATH%include\level1\Windows\ws2_32.def\" \"%TCC_PATH%include\level1\Windows\dxva2.def\" \"%TCC_PATH%include\level1\Windows\shell32.def\" \"%TCC_PATH%include\level1\Windows\ole32.def\" \"%TCC_PATH%include\level1\Windows\msvcrt.def\" \"%TCC_PATH%include\avformat.def\" \"%TCC_PATH%include\avcodec.def\" \"%TCC_PATH%include\avutil.def\" \"%TCC_PATH%include\swscale.def\""