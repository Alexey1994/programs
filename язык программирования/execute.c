#include "parser.c"
#include <file.c>


void main()
{
	C_Context context;
	Reader    reader;

	initialize_file_reader(&reader, "test");
	parse_C(&context, &reader);
	deinitialize_reader(&reader);
}