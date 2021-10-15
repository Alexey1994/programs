#ifndef LOG_INCLUDED
#define LOG_INCLUDED


#ifdef DEBUG

#include <writer.c>
#include <system/console.c>


typedef struct
{
	Number32 tail_size;
	Byte     tail[4];

	Byte* unicode_source;
	void (*write_unicode)(Byte* source, Number16 character);
}
Unicode_Source;

void initialize_Unicode_Source(Unicode_Source* source, Byte* unicode_source, void (*write_unicode)(Byte* source, Number16 character))
{
	source->tail_size      = 0;
	source->unicode_source = unicode_source;
	source->write_unicode  = write_unicode;
}

Number32 write_in_Unicode_Source(Unicode_Source* source, Byte* bytes, Number32 number_of_bytes)
{
	Byte* byte;

	while(number_of_bytes)
	{
		source->tail[source->tail_size] = *bytes;
		++bytes;
		++source->tail_size;
		--number_of_bytes;

		if(source->tail[0] < 0x80) {
			source->tail_size = 0;
			source->write_unicode(source->unicode_source, source->tail[0]);
		}
		else if((source->tail[0] & 0b11110000) && source->tail_size == 4) {
			source->tail_size = 0;
			source->write_unicode(
				source->unicode_source,
				((source->tail[0] & 0b00000111) << 18) |
					((source->tail[1] & 0b00111111) << 12) |
					((source->tail[2] & 0b00111111) << 6) |
					(source->tail[3] & 0b00111111)
			);
		}
		else if((source->tail[0] & 0b11100000) && source->tail_size == 3) {
			source->tail_size = 0;
			source->write_unicode(
				source->unicode_source,
				((source->tail[0] & 0b00001111) << 12) |
					((source->tail[1] & 0b00111111) << 6) |
					(source->tail[2] & 0b00111111)
			);
		}
		else if((source->tail[0] & 0b11000000) && source->tail_size == 2) {
			source->tail_size = 0;
			source->write_unicode(
				source->unicode_source,
				((source->tail[0] & 0b00011111) << 6) |
					(source->tail[1] & 0b00111111)
			);
		}
		
		
		if(source->tail_size > 4) {
			source->tail_size = 0;
		}
	}
}


#define print(...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	print_in_writer(&writer, ##__VA_ARGS__, 0);\
	deinitialize_writer(&writer);\
}

#define log(...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_GRAY_DARK, CONSOLE_BLACK);\
	print_in_writer(&writer, __func__, ", line ", write_Number, __LINE__, ": ", 0);\
	flush_writer(&writer);\
\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	print_in_writer(&writer, ##__VA_ARGS__, "\n", 0);\
	deinitialize_writer(&writer);\
}

#define log_error(...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_RED_DARK, CONSOLE_BLACK);\
	print_in_writer(&writer, __func__, ", line ", write_Number, __LINE__, ": ", 0);\
	flush_writer(&writer);\
\
	set_console_color(console_writer, CONSOLE_RED, CONSOLE_BLACK);\
	print_in_writer(&writer, ##__VA_ARGS__, "\n", 0);\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	deinitialize_writer(&writer);\
}
#else
#define print(...)
#define log(...)
#define log_error(...)
#endif


#endif//LOG_INCLUDED

/*
void main()
{
	log("Hi ", write_Number, 45);
	log("Hi ", write_Number, 45);
	log("Hi ", write_Number, 45);
	log("Hi ", write_Number, 45);
	log_error("Hi ", write_Number, 45);
	log("Hi ", write_Number, 45);
	log("Hi ", write_Number, 45);
}*/