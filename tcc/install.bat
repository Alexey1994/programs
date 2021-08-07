@assoc .c=
@ftype runnable_c_application=
@reg delete HKCR\.c /f
@reg delete HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\FileExts\.c /f
@reg delete HKCR\*\shell\Скомпилировать /f

@set TCC_PATH=%~dp0

@ftype runnable_c_application="%TCC_PATH%tcc.exe" -w -DDEBUG -D__WIN32__ ^
	"%TCC_PATH%include\system\Windows\kernel32.def" ^
	"%TCC_PATH%include\system\Windows\winmm.def" ^
	"%TCC_PATH%include\system\Windows\gdi32.def" ^
	"%TCC_PATH%include\system\Windows\user32.def" ^
	"%TCC_PATH%include\system\Windows\opengl32.def" ^
	"%TCC_PATH%include\system\Windows\avicap32.def" ^
	"%TCC_PATH%include\system\Windows\ws2_32.def" ^
	"%TCC_PATH%include\system\Windows\dxva2.def" ^
	"%TCC_PATH%include\system\Windows\shell32.def" ^
	"%TCC_PATH%sqlite\sqlite3.def" ^
	"%TCC_PATH%include\nodejs.def" ^
	"%TCC_PATH%include\lua.def" ^
	-run "%%1"
@assoc .c=runnable_c_application
@reg add HKCR\*\shell\Скомпилировать\command /t REG_SZ /d "\"%TCC_PATH%tcc.exe\" -w -DDEBUG -D__WIN32__ \"%%1\" \"%TCC_PATH%include\system\Windows\kernel32.def\" \"%TCC_PATH%include\system\Windows\winmm.def\" \"%TCC_PATH%include\system\Windows\gdi32.def\" \"%TCC_PATH%include\system\Windows\user32.def\" \"%TCC_PATH%include\system\Windows\opengl32.def\" \"%TCC_PATH%include\system\Windows\avicap32.def\" \"%TCC_PATH%include\system\Windows\ws2_32.def\" \"%TCC_PATH%include\system\Windows\dxva2.def\" \"%TCC_PATH%include\system\Windows\shell32.def\" \"%TCC_PATH%sqlite\sqlite3.def\" \"%TCC_PATH%include\nodejs.def\" \"%TCC_PATH%include\lua.def\""
@setx path "%PATH%;%TCC_PATH%"