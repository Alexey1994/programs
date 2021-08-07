#ifndef SYSTEM_CONSOLE_INCLUDED
#define SYSTEM_CONSOLE_INCLUDED


#include <types.c>


typedef enum
{
	CONSOLE_BLACK        = 0,
	CONSOLE_BLUE_DARK    = 1,
	CONSOLE_GREEN_DARK   = 2,
	CONSOLE_AZURE_DARK   = 3,
	CONSOLE_RED_DARK     = 4,
	CONSOLE_PINK_DARK    = 5,
	CONSOLE_YELLOW_DARK  = 6,
	CONSOLE_GRAY_DARK    = 7,
	CONSOLE_GRAY         = 8,
	CONSOLE_BLUE         = 9,
	CONSOLE_GREEN        = 10,
	CONSOLE_AZURE        = 11,
	CONSOLE_RED          = 12,
	CONSOLE_PINK         = 13,
	CONSOLE_YELLOW       = 14,
	CONSOLE_WHITE        = 15
}
Console_Color;


Byte* get_default_console_writer();
void  set_console_color(Byte* console, Console_Color text_color, Console_Color background_color);
void  write_in_console(Byte* console, Byte* charachters, Number number_of_charachters);
void  write_character_in_console(Byte* console, Number32 charachter);
Byte* get_default_console_reader();


#ifdef __WIN32__

#include <system/Windows/kernel32.c>


Byte* get_default_console_writer()
{
	return GetStdHandle(STD_OUTPUT_HANDLE);
}


static Number16 get_Windows_color(Console_Color color)
{
	switch(color)
	{
		case CONSOLE_BLACK: return 0;
		case CONSOLE_BLUE_DARK: return 1;
		case CONSOLE_GREEN_DARK: return 2;
		case CONSOLE_AZURE_DARK: return 3;
		case CONSOLE_RED_DARK: return 4;
		case CONSOLE_PINK_DARK: return 5;
		case CONSOLE_YELLOW_DARK: return 6;
		case CONSOLE_GRAY: return 7;
		case CONSOLE_GRAY_DARK: return 8;
		case CONSOLE_BLUE: return 9;
		case CONSOLE_GREEN: return 10;
		case CONSOLE_AZURE: return 11;
		case CONSOLE_RED: return 12;
		case CONSOLE_PINK: return 13;
		case CONSOLE_YELLOW: return 14;
		case CONSOLE_WHITE: return 15;
	}
}


void set_console_color(Byte* console, Console_Color text_color, Console_Color background_color)
{
	SetConsoleTextAttribute(console, get_Windows_color(text_color) + (get_Windows_color(background_color) << 4));
}


void write_in_console(Byte* console, Byte* charachters, Number number_of_charachters)
{
	Number32 bytes_written;
	WriteConsoleA(console, charachters, number_of_charachters, &bytes_written, 0);
}


void write_character_in_console(Byte* console, Number32 charachter)
{
	Number32 bytes_written;
	WriteConsoleW(console, &charachter, 1, &bytes_written, 0);
}


Byte* get_default_console_reader()
{
	return GetStdHandle(STD_INPUT_HANDLE);
}

#endif//__WIN32__


#endif//SYSTEM_CONSOLE_INCLUDED

/*
void main()
{
	Byte* console_output = get_default_console_writer();
	set_console_color(console_output, CONSOLE_WHITE, CONSOLE_BLACK);
	write_in_console(console_output, "Hi", 2);
}*/