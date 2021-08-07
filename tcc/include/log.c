#ifndef LOG_INCLUDED
#define LOG_INCLUDED


#ifdef DEBUG

#include <writer.c>
#include <system/console.c>

#define print(...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Writer writer;\
	initialize_writer(&writer, console_writer, &write_in_console);\
\
	set_console_color(console_writer, CONSOLE_WHITE, CONSOLE_BLACK);\
	print_in_writer(&writer, ##__VA_ARGS__, 0);\
	deinitialize_writer(&writer);\
}

#define log(...) \
{\
	Byte* console_writer = get_default_console_writer();\
	Writer writer;\
	initialize_writer(&writer, console_writer, &write_in_console);\
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
	Writer writer;\
	initialize_writer(&writer, console_writer, &write_in_console);\
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