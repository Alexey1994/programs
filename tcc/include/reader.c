#ifndef READER_INCLUDED
#define READER_INCLUDED


#include <data-structures/buffer.c>


typedef struct
{
	Byte*  source;
	Buffer buffer;

	Integer_Number (*read_bytes)(Byte* source, Byte* bytes, Number number_of_bytes);
	Boolean        (*end_of_data)(Byte* source);
	void           (*close_source)(Byte* source);
}
Reader;


typedef struct
{
	Reader* reader;
	Buffer  accumulator;
}
Buffered_Reader_Source;


void initialize_reader(Reader* reader, Byte* source, Integer_Number (*read_bytes)(Byte* source, Byte* bytes, Number number_of_bytes))
{
	reader->source          = source;
	reader->read_bytes      = read_bytes;
	reader->end_of_data     = 0;
	reader->close_source    = 0;

	initialize_buffer(&reader->buffer, 1);
}

void deinitialize_reader(Reader* reader)
{
	if(reader->close_source)
		reader->close_source(reader->source);

	deinitialize_buffer(&reader->buffer);
}


void read_next_byte(Reader* reader)
{
	if(reader->buffer.length)
	{
		Byte old_byte;
		remove_bytes_from_buffer_begin(&reader->buffer, &old_byte, 1);
	}
	else
	{
		if(reader->end_of_data && reader->end_of_data(reader->source))
			return;

		Byte bytes[512];
		Integer_Number bytes_readed = reader->read_bytes(reader->source, bytes, 512);
		add_bytes_in_buffer_end(&reader->buffer, bytes, bytes_readed);
	}
}


Byte get_reader_byte(Reader* reader)
{
	Byte byte;

	if(!reader->buffer.length)
		read_next_byte(reader);

	get_buffer_bytes(&reader->buffer, 0, &byte, 1);

	return byte;
}


Boolean end_of_reader(Reader* reader)
{
	if(!reader->end_of_data)
		return 0;

	if(reader->buffer.length)
		return 0;

	return reader->end_of_data(reader->source);
}


void restore_reader_from_buffer(Reader* reader, Buffer* accumulator)
{
	add_buffer_in_buffer_begin(&reader->buffer, accumulator);
}


static Integer_Number read_bytes_from_buffered_reader(Buffered_Reader_Source* source, Byte* bytes, Number bytes_length)
{
	*bytes = get_reader_byte(source->reader);
	read_next_byte(source->reader);
	add_in_buffer_end(&source->accumulator, Byte, *bytes);

	return 1;
}

static Boolean end_of_buffered_reader(Buffered_Reader_Source* source)
{
	return end_of_reader(source->reader);
}

void initialize_buffered_reader(Reader* buffered_reader, Buffered_Reader_Source* source, Reader* reader)
{
	source->reader = reader;
	initialize_buffer(&source->accumulator, 1);
	initialize_reader(buffered_reader, source, &read_bytes_from_buffered_reader);
	buffered_reader->end_of_data = &end_of_buffered_reader;
}


void deinitialize_buffered_reader(Reader* buffered_reader, Buffered_Reader_Source* source)
{
	deinitialize_buffer(&source->accumulator);
	deinitialize_reader(buffered_reader);
}


Integer_Number read_bytes(Reader* reader, Byte* bytes, Number number_of_bytes)
{
	if(reader->buffer.length)
	{
		if(reader->buffer.length >= number_of_bytes)
		{
			remove_bytes_from_buffer_begin(&reader->buffer, bytes, number_of_bytes);
			return number_of_bytes;
		}
		else
		{
			Number old_buffer_length = reader->buffer.length;
			remove_bytes_from_buffer_begin(&reader->buffer, bytes, old_buffer_length);
			return old_buffer_length + reader->read_bytes(reader->source, bytes + old_buffer_length, number_of_bytes - old_buffer_length);
		}
	}
	else
	{
		if(reader->end_of_data && reader->end_of_data(reader->source))
			return 0;

		return reader->read_bytes(reader->source, bytes, number_of_bytes);
	}
}


Number8 read_binary_Number8(Reader* reader)
{
	Number8 number = 0;
	read_bytes(reader, &number, 1);
	return number;
}


Number16 read_binary_Number16(Reader* reader)
{
	Number16 number = 0;
	read_bytes(reader, &number, 2);
	return number;
}


Number32 read_binary_Number32(Reader* reader)
{
	Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


Number64 read_binary_Number64(Reader* reader)
{
	Number64 number = 0;
	read_bytes(reader, &number, 8);
	return number;
}


Integer_Number8 read_binary_Integer_Number8 (Reader* reader)
{
	Integer_Number8 number = 0;
	read_bytes(reader, &number, 1);
	return number;
}


Integer_Number16 read_binary_Integer_Number16 (Reader* reader)
{
	Integer_Number16 number = 0;
	read_bytes(reader, &number, 2);
	return number;
}


Integer_Number32 read_binary_Integer_Number32 (Reader* reader)
{
	Integer_Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


Rational_Number32 read_binary_Rational_Number32 (Reader* reader)
{
	Rational_Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


Rational_Number64 read_binary_Rational_Number64 (Reader* reader)
{
	Rational_Number64 number = 0;
	read_bytes(reader, &number, 8);
	return number;
}


static Boolean is_number_character(char character)
{
	return character >= '0' && character <= '9';
}


Number read_Number(Reader* reader)
{
	Number number;

	number = 0;

	while(!end_of_reader(reader) && is_number_character(get_reader_byte(reader)))
	{
		number = number*10 + get_reader_byte(reader) - '0';
		read_next_byte(reader);
	}

	return number;
}


Integer_Number read_Integer_Number(Reader* reader)
{
	if(get_reader_byte(reader) == '-')
	{
		read_next_byte(reader);
		return -read_Number(reader);
	}

	return read_Number(reader);
}


Rational_Number read_Rational_Number(Reader* reader)
{
	Boolean         is_negative;
	Rational_Number decimal;
	Rational_Number fractional;
	Rational_Number basis;

	if(get_reader_byte(reader) == '-')
	{
		is_negative = 1;
		read_next_byte(reader);
	}
	else
		is_negative = 0;

	decimal = read_Number(reader);

	if(get_reader_byte(reader) != '.')
		return decimal;

	read_next_byte(reader);
	
	fractional = 0.0f;
	basis = 0.1f;
	while(!end_of_reader(reader) && is_number_character(get_reader_byte(reader)))
	{
		fractional += (Rational_Number)(get_reader_byte(reader) - '0') * basis;
		basis /= 10.0f;
		read_next_byte(reader);
	}

	if(is_negative)
		return -(decimal + fractional);
	else
		return decimal + fractional;
}


Number32 read_UTF_8_character(Reader* reader)
{
	Byte     current_byte;
	Number   number_of_bytes;
	Number32 character;
	Number   i;

	current_byte = get_reader_byte(reader);
	read_next_byte(reader);

	if(!(current_byte & 0b10000000))
		return current_byte;

	if((current_byte & 0b11110000) == 0b11110000)
	{
		number_of_bytes = 4;
		character = (current_byte & 0b00001111) << 18;
	}
	else if((current_byte & 0b11100000) == 0b11100000)
	{
		number_of_bytes = 3;
		character = (current_byte & 0b00011111) << 12;
	}
	else if((current_byte & 0b11000000) == 0b11000000)
	{
		number_of_bytes = 2;
		character = (current_byte & 0b00111111) << 6;
	}
	else
		goto error;

	for(i = 0; i < number_of_bytes - 1; ++i)
	{
		current_byte = get_reader_byte(reader);
		read_next_byte(reader);
		character |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
	}

	return character;

error:
	return 1;
}



static Boolean is_space_character(char character)
{
	return character == ' ' || character == '\r' || character == '\n' || character == '\t';
}


void read_spaces(Reader* reader)
{
	while(!end_of_reader(reader) && is_space_character(get_reader_byte(reader)))
		read_next_byte(reader);
}


static Boolean is_line_break_character(char character)
{
	return character == '\r' || character == '\n';
}


void read_line(Reader* reader)
{
	while(!end_of_reader(reader) && !is_line_break_character(get_reader_byte(reader)))
		read_next_byte(reader);

	if(get_reader_byte(reader) == '\r')
		read_next_byte(reader);

	if(get_reader_byte(reader) == '\n')
		read_next_byte(reader);
}


void scan(Reader* reader, ...)
{
	Byte** argument_address;
	Byte*  read_function;

	argument_address = &reader + 1;
    
	while(*argument_address)
	{
		read_function = *argument_address;

		if(read_function == &read_Number)
		{
			*((Number*)argument_address[1]) = read_Number(reader);
			argument_address += 2;
		}
		else if(read_function == &read_spaces)
		{
			read_spaces(reader);
			argument_address += 1;
		}
		else if(read_function == &read_line)
		{
			read_line(reader);
			argument_address += 1;
		}
		else
		{
			printf("scan function error\n");
			break;
		}
	}
}


#define READER(source, read_bytes)\
{\
	Reader reader;\
	initialize_reader(&reader, (source), (read_bytes));

#define read(type, ...)\
	read_##type(&reader, ##__VA_ARGS__)

#define END_READER\
	deinitialize_reader(&reader);\
}


#define ACCUMULATED_READER(target_reader)\
{\
	Reader* _target_reader = (target_reader);\
	{\
		Buffered_Reader_Source buffered_reader_source;\
		Reader reader;\
		initialize_buffered_reader(&reader, &buffered_reader_source, _target_reader);\
		do {\

#define restore_reader\
{\
	restore_reader_from_buffer(_target_reader, &buffered_reader_source.accumulator);\
	break;\
}

#define END_ACCUMULATED_READER\
		} while(0);\
		deinitialize_buffered_reader(&reader, &buffered_reader_source);\
	}\
}


Number32 get_reader_UTF_8_character(Reader* reader)
{
	Number32 character;

	ACCUMULATED_READER(reader)
		character = read_UTF_8_character(&reader);
		restore_reader;
	END_ACCUMULATED_READER

	return character;
}


Boolean read_if(Reader* reader, Byte* next)
{
	Boolean result;

	result = 1;

	ACCUMULATED_READER(reader)
		for(; *next && !end_of_reader(&reader); ++next)
		{
			Byte byte = get_reader_byte(&reader);

			if(byte != *next)
			{
				result = 0;
				restore_reader;
			}

			read_next_byte(&reader);
		}
	END_ACCUMULATED_READER

	return result;
}


Boolean read_if_bytes(Reader* reader, Byte* next, Number next_length)
{
	Boolean result;

	result = 1;

	ACCUMULATED_READER(reader)
		for(; next_length && !end_of_reader(&reader); ++next, --next_length)
		{
			Byte byte = get_reader_byte(&reader);

			if(byte != *next)
			{
				result = 0;
				restore_reader;
			}

			read_next_byte(&reader);
		}
	END_ACCUMULATED_READER

	return result;
}


#endif//READER_INCLUDED

/*
Integer_Number fread(Byte* buffer, Number one, Number buffer_length, Byte* file);
Boolean feof (Byte* file);

Integer_Number read_from_file(Byte* file, Byte* buffer, Number buffer_length)
{
	return fread(buffer, 1, buffer_length, file);
}


void accumulator_test()
{
	Byte* file;

	file = fopen("a.txt", "rb");

	READER(file, &read_from_file)
		reader.end_of_data = &feof;

		ACCUMULATED_READER(&reader)
			if(read(if, "123"))
				printf("%d: read_if: Ok\n", __LINE__);
			else
				printf("%d: read_if: Error\n", __LINE__);

			read(spaces);
			read(line);

			if(read(if, "456"))
				printf("%d: read_if: Ok\n", __LINE__);
			else
				printf("%d: read_if: Error\n", __LINE__);

			restore_reader;
		END_ACCUMULATED_READER

		printf("%d\n", read(Number));
		read(spaces);
		read(line);

		printf("%d\n", read(Number));
		read(spaces);

		printf("%d\n", get_reader_UTF_8_character(&reader));
		printf("%d\n", read(UTF_8_character));
		printf("%d\n", get_reader_UTF_8_character(&reader));
	END_READER

	fclose(file);
}


void scan_test()
{
	Byte* file;

	Number a;
	Number b;

	file = fopen("a.txt", "rb");

	READER(file, &read_from_file)
		reader.end_of_data = &feof;

		scan(&reader,
			read_Number, &a,
			read_spaces,
			read_line,

			read_Number, &b,
		0);

		printf("%d, %d\n", a, b);
	END_READER

	fclose(file);
}

void main()
{
	accumulator_test();
	scan_test();
}*/

/*
//#include <reader/bytes.c>
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


static Boolean end_of_null_terminated_bytes_source_data(Byte** null_terminated_bytes_source)
{
	return !**null_terminated_bytes_source;
}


void initialize_null_terminated_bytes_reader(Reader* reader, Byte* null_terminated_bytes)
{
	Byte** null_terminated_bytes_source;

	null_terminated_bytes_source = allocate_memory(sizeof(Byte**));
	*null_terminated_bytes_source = null_terminated_bytes;
	
	initialize_reader(reader, null_terminated_bytes_source, &read_bytes_from_null_terminated_bytes_source);
	reader->end_of_data = &end_of_null_terminated_bytes_source_data;
	reader->close_source = &free_memory;
}


void main()
{
	Reader reader;

	initialize_null_terminated_bytes_reader(&reader, "123 245");

	log(_Number, read_Number(&reader));
	read_spaces(&reader);
	log(_Number, read_Number(&reader));
}*/