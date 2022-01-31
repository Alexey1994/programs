#ifndef LOG_INCLUDED
#define LOG_INCLUDED


#ifdef DEBUG

#include <writer.c>
#include <console.c>


typedef struct
{
	Number tail_size;
	Byte   tail[4];

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

Number32 write_in_Unicode_Source(Unicode_Source* source, Byte* bytes, Number number_of_bytes)
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


void print(Byte* parameters, ...)
{
	Byte*          console_writer;
	Unicode_Source unicode_source;
	Writer         writer;

	console_writer = get_default_console_writer();
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);

	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);
	print_in_writer(&writer, parameters, &parameters + 1);
	deinitialize_writer(&writer);
}

void log(Byte* parameters, ...)
{
	Byte*          console_writer;
	Unicode_Source unicode_source;
	Writer         writer;
	Byte*          arguments[4];

	console_writer = get_default_console_writer();
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);

	set_console_color(console_writer, CONSOLE_GRAY_DARK, CONSOLE_BLACK);

	arguments[0] = __func__;
	arguments[1] = ", line ";
	arguments[2] = __LINE__;
	arguments[3] = ": ";
	print_in_writer(&writer, "ssns", arguments);
	flush_writer(&writer);

	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);
	print_in_writer(&writer, parameters, &parameters + 1);

	arguments[0] = "\n";
	print_in_writer(&writer, "s", arguments);
	
	deinitialize_writer(&writer);
}

void log_error(Byte* parameters, ...)
{
	Byte*          console_writer;
	Unicode_Source unicode_source;
	Writer         writer;
	Byte*          arguments[4];

	console_writer = get_default_console_writer();
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);

	set_console_color(console_writer, CONSOLE_RED_DARK, CONSOLE_BLACK);

	arguments[0] = __func__;
	arguments[1] = ", line ";
	arguments[2] = __LINE__;
	arguments[3] = ": ";
	print_in_writer(&writer, "ssns", arguments);
	flush_writer(&writer);

	set_console_color(console_writer, CONSOLE_RED, CONSOLE_BLACK);
	print_in_writer(&writer, parameters, &parameters + 1);
	
	arguments[0] = "\n";
	print_in_writer(&writer, "s", arguments);
	
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);
	deinitialize_writer(&writer);
}

/*
#define print(parameters, ...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	print_in_writer(&writer, (parameters), ##__VA_ARGS__);\
	deinitialize_writer(&writer);\
}

#define log(parameters, ...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_GRAY_DARK, CONSOLE_BLACK);\
	print_in_writer(&writer, "ssns", __func__, ", line ", __LINE__, ": ");\
	flush_writer(&writer);\
\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	print_in_writer(&writer, (parameters), ##__VA_ARGS__);\
	print_in_writer(&writer, "s", "\n");\
	deinitialize_writer(&writer);\
}

#define log_error(parameters, ...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Unicode_Source unicode_source;\
	Writer writer;\
	initialize_Unicode_Source(&unicode_source, get_default_console_writer(), &write_character_in_console);\
	initialize_writer(&writer, &unicode_source, &write_in_Unicode_Source);\
\
	set_console_color(console_writer, CONSOLE_RED_DARK, CONSOLE_BLACK);\
	print_in_writer(&writer, "ssns", __func__, ", line ", __LINE__, ": ");\
	flush_writer(&writer);\
\
	set_console_color(console_writer, CONSOLE_RED, CONSOLE_BLACK);\
	print_in_writer(&writer, (parameters), ##__VA_ARGS__);\
	print_in_writer(&writer, "s", "\n");\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	deinitialize_writer(&writer);\
}*/
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