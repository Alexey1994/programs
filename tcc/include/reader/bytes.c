#ifndef BYTES_READER_INCLUDED
#define BYTES_READER_INCLUDED


#include <reader.c>


typedef struct
{
	Byte* current_byte;
}
Null_Terminated_Bytes_Source;


static Number read_bytes_from_null_terminated_bytes_source(Byte** null_terminated_bytes_source, Byte* bytes, Number number_of_bytes)
{
	Number bytes_readed;

	for(bytes_readed = 0; bytes_readed < number_of_bytes && **null_terminated_bytes_source; ++bytes_readed)
	{
		*bytes = **null_terminated_bytes_source;
		++bytes;
		++*null_terminated_bytes_source;
	}

	return bytes_readed;
}


void initialize_null_terminated_bytes_reader(Reader* reader, Byte* null_terminated_bytes)
{
	Byte** null_terminated_bytes_source;

	null_terminated_bytes_source = allocate_memory(sizeof(Byte**));
	*null_terminated_bytes_source = null_terminated_bytes;
	
	initialize_reader(reader, null_terminated_bytes_source, &read_bytes_from_null_terminated_bytes_source);
	reader->close_source = &free_memory;
}


#endif//BYTES_READER_INCLUDED

/*
void main()
{
	Reader reader;

	initialize_null_terminated_bytes_reader(&reader, "123 245");

	log(_Number, read_Number(&reader));
	read_spaces(&reader);
	log(_Number, read_Number(&reader));
}*/