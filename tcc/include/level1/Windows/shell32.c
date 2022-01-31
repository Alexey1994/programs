#ifndef SYSTEM_WINDOWS_SHELL32_INCLUDED
#define SYSTEM_WINDOWS_SHELL32_INCLUDED


#include <types.c>

import Byte* ShellExecuteA(Byte* window, Byte* operation, Byte* file, Byte* parameters, Byte* directory, Number32 show_cmd);
import Byte* ShellExecuteW(Byte* window, Number16* operation, Number16* file, Number16* parameters, Number16* directory, Number32 show_cmd);


#endif//SYSTEM_WINDOWS_SHELL32_INCLUDED