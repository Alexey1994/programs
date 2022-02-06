#ifndef KERNEL_INCLUDED
#define KERNEL_INCLUDED


#include <types.c>


typedef void (*Module)(struct Kernel* kernel, Number module_address);


#define WRITER_BUFFER_SIZE 512

typedef struct
{
	Byte* source;

	Integer_Number (*write_bytes)(Byte* source, Byte* bytes, Number size_of_bytes);
	void           (*close_source)(Byte* source);

	Number   buffer_used;
	Byte     buffer[WRITER_BUFFER_SIZE];
}
Writer;

typedef struct
{
	Number module_address;

	void (*initialize)      (Writer* writer, Byte* source, Integer_Number (*write_bytes)(Byte* source, Byte* bytes, Number size_of_bytes));
	void (*deinitialize)    (Writer* writer);
	void (*flush)           (Writer* writer);
	void (*print)           (Writer* writer, Byte* parameters, Byte** arguments);
	void (*print_in_string) (Byte* string, Number max_size_of_string, Byte* parameters, Byte** arguments);
}
Writer_Context;


typedef struct
{
	Number module_address;

	void (*print)(struct Kernel* kernel, Byte* parameters, ...);
	void (*clear)(struct Kernel* kernel);

	Bit16*   address;
	Number32 x_coord;
	Number32 y_coord;
	Number32 width;
	Number32 height;

	Writer   writer;
}
Console_Context;


typedef struct
{
	Number module_address;

	Boolean (*set_mode) (Number width, Number height, Number bits_per_pixel, Number frequency);
}
Display_Context;


typedef struct
{
	Byte*  (*allocate_global_memory) (Number size);
	Module (*load_module)            (Number16* directory, Number16* path);

	Writer_Context*  writer;
	Console_Context* console;
	Display_Context* display;

/*
	Number memory_size;

	Number8  (*in_8)   (Number16 port);
	void     (*out_8)  (Number16 port, Number8 data);
	Number16 (*in_16)  (Number16 port);
	void     (*out_16) (Number16 port, Number16 data);
	Number32 (*in_32)  (Number16 port);
	void     (*out_32) (Number16 port, Number32 data);

	void (*add_interrupt_handler) (Number8 interrupt_number, void(*handler)(Byte* data), Byte* data);
*/
}
Kernel;


#endif//KERNEL_INCLUDED