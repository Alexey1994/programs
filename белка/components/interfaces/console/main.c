#include <kernel.h>


void print(Kernel* kernel, Byte* parameters, ...);
void clear(Kernel* kernel);

static Number32 write_bytes_in_console(Console_Context* console, Byte* bytes, Number bytes_length);


void start_module(Kernel* kernel, Number module_address)
{
	Console_Context* console;
	Writer_Context*  writer;

	console = kernel->console = kernel->allocate_global_memory(sizeof(Console_Context));
	writer  = kernel->writer;

	console->module_address = module_address;

	console->print = module_address + (Byte*)&print;
	console->clear = module_address + (Byte*)&clear;

	console->address = 0xB8000;
	console->x_coord = 0;
	console->y_coord = 0;
	console->width   = 80;
	console->height  = 25;

	console->clear(kernel);

	writer->initialize(&console->writer, console, (Byte*)&write_bytes_in_console + module_address);
}


static void write_byte(Console_Context* console, Byte byte)
{
	Number i;

	if(console->x_coord == console->width)
	{
		console->x_coord = 0;
		++console->y_coord;
	}

	if(console->y_coord == console->height)
	{
		//shift up
		for(i = 0; i < console->width * (console->height - 1); ++i)
			console->address[i] = console->address[i + console->width];

		//clear last line
		for(i = console->width * (console->height - 1); i < console->width * console->height; ++i)
			console->address[i] = 0;

		--console->y_coord;
	}

	if(byte == '\r')
		return;

	if(byte == '\t')
	{
		for(i = 0; i < 4; ++i)
			write_byte(console, ' ');
		return;
	}

	if(byte == '\n')
	{
		++console->y_coord;
		console->x_coord = 0;
	}
	else
	{
		console->address[console->y_coord * console->width + console->x_coord] = byte + (15 << 8);
		++console->x_coord;
	}
}


static Number32 write_bytes_in_console(Console_Context* console, Byte* bytes, Number bytes_length)
{
	Number i;

	for(i = 0; i < bytes_length; ++i)
		write_byte(console, bytes[i]);

	return bytes_length;
}


void print(Kernel* kernel, Byte* parameters, ...)
{
	Console_Context* console;
	Writer_Context*  writer;

	console = kernel->console;
	writer = kernel->writer;

	writer->print(&console->writer, parameters, &parameters + 1);
	writer->flush(&console->writer);
}


void clear(Kernel* kernel)
{
	Console_Context* console;
	Number           i;

	console = kernel->console;

	console->x_coord = 0;
	console->y_coord = 0;

	for(i = 0; i < console->width * console->height; ++i)
		console->address[i] = 0;
}