#include <kernel.h>


static void initialize_writer(Writer* writer, Byte* source, Integer_Number (*write_bytes)(Byte* source, Byte* bytes, Number size_of_bytes));
static void deinitialize_writer(Writer* writer);
static void flush_writer(Writer* writer);
static void print_in_writer(Writer* writer, Byte* parameters, Byte** arguments);
static void print_in_null_terminated_bytes(Byte* bytes, Number max_size, Byte* parameters, ...);


void start_module(Kernel* kernel, Number module_address)
{
	Writer_Context* writer;

	writer = kernel->writer = kernel->allocate_global_memory(sizeof(Writer_Context));

	writer->module_address = module_address;

	writer->initialize      = module_address + (Byte*)&initialize_writer;
	writer->deinitialize    = module_address + (Byte*)&deinitialize_writer;
	writer->flush           = module_address + (Byte*)&flush_writer;
	writer->print           = module_address + (Byte*)&print_in_writer;
	writer->print_in_string = module_address + (Byte*)&print_in_null_terminated_bytes;
}


void initialize_writer(Writer* writer, Byte* source, Integer_Number (*write_bytes)(Byte* source, Byte* bytes, Number size_of_bytes))
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


static void write_Byte(Writer* writer, Byte byte)
{
	writer->buffer[writer->buffer_used] = byte;
	++writer->buffer_used;

	if(writer->buffer_used == WRITER_BUFFER_SIZE)
	{
		writer->write_bytes(writer->source, writer->buffer, WRITER_BUFFER_SIZE);
		writer->buffer_used = 0;
	}
}


static void write_Number(Writer* writer, Number number)
{
	Number number_copy;
	Number right_selector;

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


static void write_Integer_Number(Writer* writer, Integer_Number number)
{
	Number number_copy;
	Number right_selector;

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


static void write_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Number i;

	for(i = 0; i < number_of_bytes; ++i)
		write_Byte(writer, bytes[i]);
}


static void write_bytes_in_big_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
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


static void write_bytes_in_little_endian(Writer* writer, Byte* bytes, Number number_of_bytes)
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


static void write_null_terminated_bytes(Writer* writer, Byte* bytes)
{
	for(; *bytes; ++bytes)
		write_Byte(writer, *bytes);
}


static void write_binary_Number32(Writer* writer, Number32 number)
{
	write_bytes(writer, &number, 4);
}


static void write_binary_Number16(Writer* writer, Number16 number)
{
	write_bytes(writer, &number, 2);
}


static void write_binary_Number8(Writer* writer, Number8 number)
{
	write_bytes(writer, &number, 1);
}


static void write_binary_Rational_Number32(Writer* writer, Rational_Number32 number)
{
	write_bytes(writer, &number, 4);
}


static void write_hex_character(Writer* writer, Byte character)
{
	if(character < 10)
		write_Byte(writer, character + '0');
	else
		write_Byte(writer, character - 10 + 'A');
}


static void write_hex_byte(Writer* writer, Byte byte)
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


static void write_hex_bytes(Writer* writer, Byte* bytes, Number number_of_bytes)
{
	Number i;

	for(i=0; i<number_of_bytes; ++i)
		write_hex_byte(writer, bytes[i]);
}


static void write_hex_Number8(Writer* writer, Number8 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[0]);
}


static void write_hex_Number16(Writer* writer, Number16 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


static void write_hex_Number32(Writer* writer, Number32 number)
{
	Byte* bytes;

	bytes = &number;
	write_null_terminated_bytes(writer, "0x");
	write_hex_byte(writer, bytes[3]);
	write_hex_byte(writer, bytes[2]);
	write_hex_byte(writer, bytes[1]);
	write_hex_byte(writer, bytes[0]);
}


static void write_bit8(Writer* writer, Number8 number)
{
	Number i;

	for(i = 8; i; --i)
		if(number & (1 << (i - 1)))
			write_Byte(writer, '1');
		else
			write_Byte(writer, '0');
}


static void print_in_writer(Writer* writer, Byte* parameters, Byte** arguments)
{
	Byte   parameter;
	Byte*  read_function;

	for(;;)
	{
		parameter = *parameters;

		if(!parameter)
			break;

		++parameters;

		if(parameter == '%')
		{
			parameter = *parameters;
			++parameters;

			switch(parameter)
			{
				case 'c':
					write_Byte(writer, *arguments);
					++arguments;
					break;

				case 'x':
					write_hex_byte(writer, *arguments);
					++arguments;
					break;

				case 'u':
					write_Number(writer, *arguments);
					++arguments;
					break;

				case 'd':
					write_Integer_Number(writer, *arguments);
					++arguments;
					break;

				case 's':
					write_null_terminated_bytes(writer, *arguments);
					++arguments;
					break;

				default:
					write_Byte(writer, parameter);
			}
		}
		else
		{
			write_Byte(writer, parameter);
		}
	}
}


typedef struct
{
	Byte* bytes;
	Number max_size;
}
Null_Terminated_Bytes_Source;

static void write_bytes_in_null_terminated_bytes(Null_Terminated_Bytes_Source* source, Byte* bytes, Number number_of_bytes)
{
	if(source->max_size < number_of_bytes)
	{
		//log_error("s", "Превышено количество байт");
		return;
	}

	while(number_of_bytes)
	{
		*source->bytes = *bytes;
		++source->bytes;
		++bytes;
		--source->max_size;
		--number_of_bytes;
	}
}

static void print_in_null_terminated_bytes(Byte* bytes, Number max_size, Byte* parameters, ...)
{
	Writer                       writer;
	Null_Terminated_Bytes_Source source;

	source.bytes    = bytes;
	source.max_size = max_size;

	initialize_writer(&writer, &source, &write_bytes_in_null_terminated_bytes);
	print_in_writer(&writer, parameters, &parameters + 1);
	write_Byte(&writer, '\0');
	flush_writer(&writer);
}