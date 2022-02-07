#include <types.c>
#include <Windows/kernel32.c>
#include <string.c>


Number32 execute(Byte* command)
{
	Sturtup_Info        startup_info = {.size = sizeof(Sturtup_Info)};
	Process_Information process_info;
	
	Number16* unicode_command = convert_utf8_to_unicode(command);
	Number32 state = CreateProcessW(0, unicode_command, 0, 0, 0, 0, 0, 0, &startup_info, &process_info);
	free_memory(unicode_command);

	if(!state)
		return 1;

	WaitForSingleObject(process_info.process, 0xFFFFFFFF);

	Number32 exit_code = 0;
	if(!GetExitCodeProcess(process_info.process, &exit_code))
		return 1;

	return exit_code;
}


Number32 main(Number number_of_arguments, Byte** arguments)
{
	Byte*  module_name;
	Byte   buffer[512];
	Number exit_code;

	if(number_of_arguments != 2)
		return 1;

	module_name = get_utf8_argument(arguments[1]);
	exit_code = 0;

	snprintf(buffer, 512, "cmd /c \"mkdir \"bin\\%s\"\"", module_name);
	execute(buffer);

	snprintf(buffer, 512, "cmd /c \"tcc32.exe -Iglobal \"%s\\main.c\" -w -c -nostdlib -fno-builtin -o \"bin\\%s\\out.elf\"\"", module_name, module_name);
	exit_code = execute(buffer);
	if(exit_code)
		goto end;

	snprintf(buffer, 512, "cmd /c \"ld -T module_script.ld -o \"bin\\%s\\out.o\" \"bin\\%s\\out.elf\"\"", module_name, module_name);
	exit_code = execute(buffer);
	if(exit_code)
		goto end;

	snprintf(buffer, 512, "cmd /c \"objcopy -O binary -S \"bin\\%s\\out.o\" \"bin\\%s\\out\"\"", module_name, module_name);
	exit_code = execute(buffer);
	if(exit_code)
		goto end;

end:
	free_memory(module_name);

	return exit_code;
}