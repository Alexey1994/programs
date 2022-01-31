#ifndef READER_INCLUDED
#define READER_INCLUDED


#include <types.c>


#define SIZE_OF_READER_BUFFER 16


typedef struct
{
	Byte*  source;

	Integer_Number (*read_bytes)(Byte* source, Byte* bytes, Number number_of_bytes);
	//Boolean        (*end_of_data)(Byte* source);
	void           (*close_source)(Byte* source);

	Boolean end_of_data;
	Boolean accumulate_bytes;
	Number  accumulator_start_index;
	Number  size_of_buffer;
	Number  start_index;
	Number  end_index;
	Byte    buffer[SIZE_OF_READER_BUFFER];
}
Reader;


static void initialize_reader(Reader* reader, Byte* source, Integer_Number (*read_bytes)(Byte* source, Byte* bytes, Number number_of_bytes))
{
	reader->source           = source;
	reader->read_bytes       = read_bytes;
	reader->end_of_data      = 0;
	reader->close_source     = 0;
	reader->accumulate_bytes = 0;
	reader->size_of_buffer   = 0;
	reader->start_index      = 0;
	reader->end_index        = 0;
}


static void deinitialize_reader(Reader* reader)
{
	if(reader->close_source)
		reader->close_source(reader->source);
}


static Integer_Number read_next_byte(Reader* reader)
{
	Integer_Number bytes_readed;

	bytes_readed = 0;

	if(reader->size_of_buffer)
	{
		--reader->size_of_buffer;

		if(reader->start_index != reader->end_index)
		{
			++reader->start_index;

			if(reader->start_index >= SIZE_OF_READER_BUFFER)
				reader->start_index = 0;
		}
	}
	else if(reader->accumulate_bytes)
	{
		if(reader->end_of_data)
			return 0;

		bytes_readed = reader->read_bytes(reader->source, reader->buffer + reader->end_index, 1);

		if(bytes_readed > 0)
		{
			reader->size_of_buffer += bytes_readed;
			reader->end_index += bytes_readed;

			if(reader->end_index >= SIZE_OF_READER_BUFFER)
				reader->end_index = reader->end_index - SIZE_OF_READER_BUFFER;
		}
	}
	else
	{
		if(reader->end_of_data)
			return 0;

		bytes_readed = reader->read_bytes(reader->source, reader->buffer, SIZE_OF_READER_BUFFER);

		if(bytes_readed < SIZE_OF_READER_BUFFER)
			reader->end_of_data = 1;

		reader->start_index = 0;

		if(bytes_readed > 0)
		{
			reader->size_of_buffer = bytes_readed;
			reader->end_index = bytes_readed - 1;
		}
		else
			reader->end_index = 0;
	}

	return bytes_readed;
}


static Byte get_reader_byte(Reader* reader)
{
	if(!reader->size_of_buffer)
		read_next_byte(reader);

	return reader->buffer[reader->start_index];
}


static Boolean end_of_reader(Reader* reader)
{
	if(reader->size_of_buffer)
		return 0;

	return reader->end_of_data;
}


static Boolean read_if(Reader* reader, Byte* null_terminated_bytes)
{
	Byte byte1;
	Byte byte2;

	reader->accumulate_bytes = 1;
	reader->accumulator_start_index = reader->start_index;

	Number bytes_readed = 0;

	for(;;)
	{
		byte1 = *null_terminated_bytes;

		if(!byte1)
		{
			reader->accumulate_bytes = 0;
			return 1;
		}

		byte2 = get_reader_byte(reader);
		read_next_byte(reader);
		++bytes_readed;
		
		if(byte1 != byte2)
		{
			reader->accumulate_bytes = 0;
			reader->start_index = reader->accumulator_start_index;
			reader->size_of_buffer += bytes_readed;
			return 0;
		}

		++null_terminated_bytes;
	}
}


static Boolean read_if_bytes(Reader* reader, Byte* bytes, Number number_of_bytes)
{
	Byte byte1;
	Byte byte2;

	if(!number_of_bytes)
		return 1;

	reader->accumulate_bytes = 1;
	reader->accumulator_start_index = reader->start_index;

	Number bytes_readed = 0;

	while(number_of_bytes)
	{
		byte1 = *bytes;

		byte2 = get_reader_byte(reader);
		read_next_byte(reader);
		++bytes_readed;
		
		if(byte1 != byte2)
		{
			reader->accumulate_bytes = 0;
			reader->start_index = reader->accumulator_start_index;
			reader->size_of_buffer += bytes_readed;
			return 0;
		}

		++bytes;
		--number_of_bytes;
	}

	reader->accumulate_bytes = 0;
	return 1;
}


static Integer_Number read_bytes(Reader* reader, Byte* bytes, Number number_of_bytes)
{
	/*
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
	*/

	Integer_Number bytes_readed;

	bytes_readed = 0;

	while(number_of_bytes)
	{
		*bytes = get_reader_byte(reader);

		++bytes;
		++bytes_readed;
		--number_of_bytes;
		read_next_byte(reader);
	}

	return bytes_readed;
}


static Number8 read_binary_Number8(Reader* reader)
{
	Number8 number = 0;
	read_bytes(reader, &number, 1);
	return number;
}


static Number16 read_binary_Number16(Reader* reader)
{
	Number16 number = 0;
	read_bytes(reader, &number, 2);
	return number;
}


static Number32 read_binary_Number32(Reader* reader)
{
	Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


static Integer_Number8 read_binary_Integer_Number8 (Reader* reader)
{
	Integer_Number8 number = 0;
	read_bytes(reader, &number, 1);
	return number;
}


static Integer_Number16 read_binary_Integer_Number16 (Reader* reader)
{
	Integer_Number16 number = 0;
	read_bytes(reader, &number, 2);
	return number;
}


static Integer_Number32 read_binary_Integer_Number32 (Reader* reader)
{
	Integer_Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


static Rational_Number32 read_binary_Rational_Number32 (Reader* reader)
{
	Rational_Number32 number = 0;
	read_bytes(reader, &number, 4);
	return number;
}


static Rational_Number64 read_binary_Rational_Number64 (Reader* reader)
{
	Rational_Number64 number = 0;
	read_bytes(reader, &number, 8);
	return number;
}


static Boolean is_number_character(char character)
{
	return character >= '0' && character <= '9';
}


static Number read_Number(Reader* reader)
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


static Integer_Number read_Integer_Number(Reader* reader)
{
	if(get_reader_byte(reader) == '-')
	{
		read_next_byte(reader);
		return -read_Number(reader);
	}

	return read_Number(reader);
}


static Rational_Number read_Rational_Number(Reader* reader)
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


static Number32 read_UTF_8_character(Reader* reader)
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


static Number32 get_reader_UTF_8_character(Reader* reader)
{
	reader->accumulate_bytes = 1;
	reader->accumulator_start_index = reader->start_index;


	Byte     current_byte;
	Number   number_of_bytes;
	Number32 character;
	Number   i;

	current_byte = get_reader_byte(reader);
	read_next_byte(reader);
	number_of_bytes = 1;

	if(!(current_byte & 0b10000000))
		character = current_byte;
	else
	{
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
			character = 0;

		for(i = 0; i < number_of_bytes - 1; ++i)
		{
			current_byte = get_reader_byte(reader);
			read_next_byte(reader);
			character |= (current_byte & 0b00111111) << ((number_of_bytes - 2 - i) * 6);
		}
	}


	reader->accumulate_bytes = 0;
	reader->start_index = reader->accumulator_start_index;
	reader->size_of_buffer += number_of_bytes;

	return character;
}


static Boolean is_space_character(char character)
{
	return character == ' ' || character == '\r' || character == '\n' || character == '\t';
}


static void read_spaces(Reader* reader)
{
	while(!end_of_reader(reader) && is_space_character(get_reader_byte(reader)))
		read_next_byte(reader);
}


static Boolean is_line_break_character(char character)
{
	return character == '\r' || character == '\n';
}


static void read_line(Reader* reader)
{
	while(!end_of_reader(reader) && !is_line_break_character(get_reader_byte(reader)))
		read_next_byte(reader);

	if(get_reader_byte(reader) == '\r')
		read_next_byte(reader);

	if(get_reader_byte(reader) == '\n')
		read_next_byte(reader);
}


static void scan(Reader* reader, ...)
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
			print("scan function error\n");
			break;
		}
	}
}


#endif//READER_INCLUDED

/*
#include <system/memory.c>
#include <reader/bytes.c>


void main()
{
	Reader reader;

	initialize_null_terminated_bytes_reader(&reader, "123 245");

	read_if(&reader, "1");
	read_if(&reader, "234");
	read_if(&reader, "234");

	print(_Number, read_Number(&reader), "\n");
	read_spaces(&reader);
	print(_Number, read_Number(&reader), "\n");

	print(
		reader.buffer, "\n",
		_Number, reader.start_index,
		_Number, reader.end_index,
		_Number, reader.size_of_buffer
	);
}*/