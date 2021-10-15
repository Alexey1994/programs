#ifndef WRITER_INCLUDED
#define WRITER_INCLUDED


#include <types.c>


#define OUTPUT_BUFFER_SIZE 512


typedef struct
{
	Byte* source;

	Integer_Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 size_of_bytes);
	void             (*close_source)(Byte* source);

	Number   buffer_used;
	Byte     buffer[OUTPUT_BUFFER_SIZE];
}
Writer;


void initialize_writer(Writer* writer, Byte* source, Integer_Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 size_of_bytes))
{
	writer->buffer_used  = 0;
	writer->source       = source;
	writer->write_bytes  = write_bytes;
	writer->close_source = 0;
}


void deinitialize_writer(Writer* writer)
{
	if(writer->buffer_used)
		writer->write_bytes(writer->source, writer->buffer, writer->buffer_used);

	if(writer->close_source)
		writer->close_source(writer->source);
}


void flush_writer(Writer* writer)
{
	if(writer->buffer_used)
	{
		writer->write_bytes(writer->source, writer->buffer, writer->buffer_used);
		writer->buffer_used = 0;
	}
}


void write_Byte(Writer* writer, Byte byte)
{
	writer->buffer[writer->buffer_used] = byte;
	++writer->buffer_used;

	if(writer->buffer_used == OUTPUT_BUFFER_SIZE)
	{
		writer->write_bytes(writer->source, writer->buffer, OUTPUT_BUFFER_SIZE);
		writer->buffer_used = 0;
	}
}


void write_Number(Writer* writer, Number number)
{
	Number32 number_copy;
	Number32 right_selector;

	number_copy    = number;
	right_selector = 1;

	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
	}

	do
	{
		write_Byte(writer, number / right_selector % 10 + '0');
		right_selector /= 10;
	}
	while(right_selector);
}


void write_Number64(Writer* writer, Number64 number)
{
#ifdef __WIN32__
	Number64 number_copy;
	Number64 right_selector;

	number_copy    = number;
	right_selector = 1;

	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
	}

	do
	{
		write_Byte(writer, number / right_selector % 10 + '0');
		right_selector /= 10;
	}
	while(right_selector);
#endif
}


void write_Integer_Number(Writer* writer, Integer_Number number)
{
	Number32 number_copy;
	Number32 right_selector;

	if(number < 0)
	{
		write_Byte(writer, '-');
		number = -number;
	}

	number_copy    = number;
	right_selector = 1;

	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
	}

	do
	{
		write_Byte(writer, number / right_selector % 10 + '0');
		right_selector /= 10;
	}
	while(right_selector);
}

/*
void write_Rational_Number(Writer* writer, Rational_Number number)
{
	Number   number_copy;
	Number32 right_selector;

	if(number < 0)
	{
		write_Byte(writer, '-');
		number = -number;
	}

	number_copy    = number;
	right_selector = 1;

	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
	}

	do
	{
		write_Byte(writer, (Number)number / right_selector % 10 + '0');
		right_selector /= 10;
	}
	while(right_selector);
}*/


void write_Number64_triplets(Writer* writer, Number64 number)
{
#ifdef __WIN32__
	Number64 number_copy;
	Number64 right_selector;
	Number   i;
	Boolean  is_first_triplet = 1;

	number_copy    = number;
	right_selector = 1;

	i = 1;
	for(;;)
	{
		number_copy /= 10;

		if(!number_copy)
			break;

		right_selector *= 10;
		++i;
	}

	do
	{
		if(!(i % 3) && !is_first_triplet)
			write_Byte(writer, ' ');
		
		is_first_triplet = 0;

		write_Byte(writer, number / right_selector % 10 + '0');
		right_selector /= 10;
		--i;
	}
	while(right_selector);
#endif
}


void write_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Number i;

	for(i = 0; i < number_of_bytes; ++i)
		write_Byte(writer, bytes[i]);
}


void write_bytes_in_big_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Bit16  a = 1;
	Number i;

	if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
	{
		for(i = number_of_bytes; i; --i)
			write_Byte(writer, bytes[i - 1]);
	}
	else
	{
		for(i = 0; i < number_of_bytes; ++i)
			write_Byte(writer, bytes[i]);
	}
}


void write_bytes_in_little_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Bit16  a = 1;
	Number i;

	if(((Bit8*)&a)[0] > ((Bit8*)&a)[1]) //little
	{
		for(i = 0; i < number_of_bytes; ++i)
			write_Byte(writer, bytes[i]);
	}
	else
	{
		for(i = number_of_bytes; i; --i)
			write_Byte(writer, bytes[i - 1]);
	}
}


void write_null_terminated_bytes(Writer* writer, Byte* bytes)
{
	for(; *bytes; ++bytes)
		write_Byte(writer, *bytes);
}


void write_null_terminated_words(Writer* writer, Number16* words)
{
	for(; *words; ++words)
		write_Byte(writer, *words);
}


void write_binary_Number32(Writer* writer, Number32 number)
{
	write_bytes(writer, &number, 4);
}


void write_binary_Number16(Writer* writer, Number16 number)
{
	write_bytes(writer, &number, 2);
}


void write_binary_Number8(Writer* writer, Number8 number)
{
	write_bytes(writer, &number, 1);
}


void write_binary_Rational_Number32(Writer* writer, Rational_Number32 number)
{
	write_bytes(writer, &number, 4);
}


void write_hex_character(Writer* writer, Byte character)
{
	if(character < 10)
		write_Byte(writer, character + '0');
	else
		write_Byte(writer, character - 10 + 'A');
}


void write_hex_byte(Writer* writer, Byte byte)
{
	if(byte < 16)
	{
		write_Byte(writer, '0');
		write_hex_character(writer, byte);
	}
	else
	{
		write_hex_character(writer, byte >> 4);
		write_hex_character(writer, byte & 0b00001111);
	}
}


void write_hex_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Number i;

	for(i=0; i<number_of_bytes; ++i)
		write_hex_byte(writer, bytes[i]);
}


void write_hex_Number8(Writer* writer, Number8 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number16(Writer* writer, Number16 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


void write_hex_Number32(Writer* writer, Number32 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[3]);
	write_hex_byte(writer, bytes[2]);
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


void write_bit8(Writer* writer, Number8 number)
{
	Number i;

	for(i = 8; i; --i)
		if(number & (1 << (i - 1)))
			write_Byte(writer, '1');
		else
			write_Byte(writer, '0');
}


Byte* _Number = &write_Number;
Byte* _Number64 = &write_Number64;
Byte* _Number64_triplets = &write_Number64_triplets;
Byte* _hex_Number32 = &write_hex_Number32;
Byte* _hex_Number16 = &write_hex_Number16;
Byte* _null_terminated_words = &write_null_terminated_words;
//Byte* _Rational_Number = &write_Rational_Number;


void print_in_writer(Writer* writer, ...)
{
	Byte** argument_address;
	Byte*  read_function;

	argument_address = (Number)&writer + sizeof(Number);
    
	for(;;)
	{
		read_function = *argument_address;

		if(!read_function)
			break;

		if(read_function == &write_Number)
		{
			write_Number(writer, argument_address[1]);
			argument_address += 2;
		}
		/*else if(read_function == &write_Rational_Number)
		{
			write_Rational_Number(writer, argument_address[1]);
			argument_address += 2;
		}*/
		else if(read_function == &write_Number64)
		{
			write_Number64(writer, *((Number64*)(argument_address + 1)));
			argument_address += 3;
		}
		else if(read_function == &write_Number64_triplets)
		{
			write_Number64_triplets(writer, *((Number64*)(argument_address + 1)));
			argument_address += 3;
		}
		else if(read_function == &write_hex_Number32)
		{
			write_hex_Number32(writer, argument_address[1]);
			argument_address += 2;
		}
		else if(read_function == &write_hex_Number16)
		{
			write_hex_Number16(writer, argument_address[1]);
			argument_address += 2;
		}
		else if(read_function == &write_null_terminated_words)
		{
			write_null_terminated_words(writer, argument_address[1]);
			argument_address += 2;
		}
		else
		{
			write_null_terminated_bytes(writer, argument_address[0]);
			argument_address += 1;
		}
	}
}


#define WRITER(source, write_bytes_in_source)\
{\
	Writer writer;\
	initialize_writer(&writer, (source), (write_bytes_in_source));

#define write(type, ...)\
	write_##type(&writer, ##__VA_ARGS__)

#define END_WRITER\
	deinitialize_writer(&writer);\
}


#endif//WRITER_INCLUDED

/*
#include <writer/bytes.c>


void out_test()
{
	Byte   bytes[512];
	Writer writer;

	initialize_bytes_writer(&writer, bytes, 512);
	{
		write(null_terminated_bytes, "Hi! ");
		write(Number, 123);
		write(Byte, 0);
	}
	deinitialize_writer(&writer);

	printf("%s\n", bytes);
}

void print_test()
{
	Byte   bytes[512];
	Writer writer;

	initialize_bytes_writer(&writer, bytes, 512);
	{
		print_in_writer(&writer,
			"Hi! ",
			write_Number, 123,
			" number",
		0);
		write(Byte, 0);
	}
	deinitialize_writer(&writer);

	printf("%s\n", bytes);
}

void main ()
{
	out_test();
	print_test();
}*/