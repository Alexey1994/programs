#ifndef TEXT_SCREEN_INCLUDED
#define TEXT_SCREEN_INCLUDED


#include <writer.c>
#include "IO.c"


typedef enum
{                             //B    G    R
	CONSOLE_BLACK        = 0, //0    0    0
	CONSOLE_BLUE_DARK    = 1, //0.5  0    0
	CONSOLE_GREEN_DARK   = 2, //0    0.5  0
	CONSOLE_CYAN_DARK    = 3, //0.5  0.5  0
	CONSOLE_RED_DARK     = 4, //0    0    0.5
	CONSOLE_PINK_DARK    = 5, //0.5  0    0.5
	CONSOLE_YELLOW_DARK  = 6, //0    0.5  0.5
	CONSOLE_WHITE_DARK   = 7, //0.5  0.5  0.5

	CONSOLE_GRAY         = 8, //0    0    0
	CONSOLE_BLUE         = 9, //1    0    0
	CONSOLE_GREEN        = 10,//0    1    0
	CONSOLE_CYAN         = 11,//1    1    0
	CONSOLE_RED          = 12,//0    0    1
	CONSOLE_PINK         = 13,//1    0    1
	CONSOLE_YELLOW       = 14,//0    1    1
	CONSOLE_WHITE        = 15 //1    1    1
}
Console_Color;


typedef struct
{
	Bit16*   address;
	Number32 x_coord;
	Number32 y_coord;
	Number32 width;
	Number32 height;
}
Text_Screen;


Text_Screen default_text_screen;
Writer      default_text_screen_writer;


void initialize_text_screen(Text_Screen* text_screen, Byte* address, Number32 width, Number32 height)
{
	text_screen->address = address;
	text_screen->x_coord = 0;
	text_screen->y_coord = 0;
	text_screen->width   = width;
	text_screen->height  = height;
}


void hide_text_screen_cursor()
{
	out_8(0x3D4, 0x0A);
	out_8(0x3D5, 0x20);
}


void set_text_screen_character(Number x, Number y, Byte character, Byte background_color, Byte text_color)
{
	/*Number16* screen;

	screen = 0xB8000;
	screen[y * 80 + x] = character + (text_color << 8) + (background_color << 12);*/

	default_text_screen.address[y * default_text_screen.width + x] = character;
	draw_character(x, y, character);
}


void clear_text_screen()
{
	VESA_Mode_Info* video_mode = 0x7D00;
	//draw_VESA_rectangle(video_mode, 0, 0, video_mode->width, video_mode->height, CONSOLE_BLACK);

	Number i;

	default_text_screen.x_coord = 0;
	default_text_screen.y_coord = 0;

	for(i = 0; i < default_text_screen.width * default_text_screen.height; ++i)
		default_text_screen.address[i] = 0;
}


void clear_text_screen_buffer()
{
	default_text_screen.x_coord = 0;
	default_text_screen.y_coord = 0;

	VESA_Mode_Info* video_mode = 0x7D00;
	//draw_VESA_rectangle(video_mode, 0, CHARACTER_HEIGHT, video_mode->width, video_mode->height - CHARACTER_HEIGHT, 0);

	Number i;

	for(i = 0; i < default_text_screen.width * default_text_screen.height; ++i)
		default_text_screen.address[i] = 0;
}


void write_byte_in_text_screen(Text_Screen* screen, Byte byte)
{
	Number i;

	if(screen->x_coord == screen->width)
	{
		screen->x_coord = 0;
		++screen->y_coord;
	}

	if(screen->y_coord == screen->height)
	{
		Number i;
		Number j;

		//shift up
		for(i = 0; i < screen->width * (screen->height - 1); ++i)
			screen->address[i] = screen->address[i + screen->width];

		//clear last line
		for(i = screen->width * (screen->height - 1); i < screen->width * screen->height; ++i)
			screen->address[i] = 0;

		for(i = 0; i < screen->height; ++i)
			for(j = 0; j < screen->width; ++j)
				set_text_screen_character(j, i, screen->address[i * screen->width + j], CONSOLE_BLACK, CONSOLE_WHITE);

		--screen->y_coord;
	}

	if(byte == '\r')
		return;

	if(byte == '\t')
	{
		for(i = 0; i < 4; ++i)
			write_byte_in_text_screen(screen, ' ');
		return;
	}

	if(byte == '\n')
	{
		++screen->y_coord;
		screen->x_coord = 0;
	}
	else
	{
		screen->address[screen->y_coord * screen->width + screen->x_coord] = byte;
		set_text_screen_character(screen->x_coord, screen->y_coord, byte, CONSOLE_BLACK, CONSOLE_WHITE);
		++screen->x_coord;
	}

/*
	if(screen->y_coord == screen->height)
	{
		Number i;

		//shift up
		for(i = 0; i < screen->width * (screen->height - 1); ++i)
			screen->address[i] = screen->address[i + screen->width];

		//clear last line
		for(i=screen->width * (screen->height - 1); i < screen->width * screen->height; ++i)
			screen->address[i] = 0;

		--screen->y_coord;
	}*/
}


Number32 write_bytes_in_text_screen(Text_Screen* screen, Byte* bytes, Number32 bytes_length)
{
	Number i;
	for(i = 0; i < bytes_length; ++i)
		write_byte_in_text_screen(screen, bytes[i]);

	return bytes_length;
}

Number16 screen[113 * 39];
void initialize_default_text_screen()
{
	initialize_text_screen(&default_text_screen, screen, 113, 39);//0xB8000, 80, 24);
	initialize_writer(&default_text_screen_writer, &default_text_screen, &write_bytes_in_text_screen);
}


//#define print(...) {print_in_writer(&default_text_screen_writer, ##__VA_ARGS__, 0); flush_writer(&default_text_screen_writer);}
#undef print

void print(Byte* chunk, ...)
{
	Byte** arguments;

	arguments = &chunk + 1;

	while(*chunk)
	{
		switch(*chunk)
		{
			case 's':
				write_null_terminated_bytes(&default_text_screen_writer, *arguments);
				++arguments;
				break;

			case 'S':
				write_null_terminated_words(&default_text_screen_writer, *arguments);
				++arguments;
				break;

			case 'n':
				write_Number(&default_text_screen_writer, *arguments);
				++arguments;
				break;

			case 'h':
				write_hex_Number32(&default_text_screen_writer, *arguments);
				++arguments;
				break;

			case 'c':
				write_Byte(&default_text_screen_writer, *arguments);
				++arguments;
				break;
		}

		++chunk;
	}

	flush_writer(&default_text_screen_writer);
}


#endif//TEXT_SCREEN_INCLUDED