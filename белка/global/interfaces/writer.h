#define WRITER_BUFFER_SIZE 512


typedef enum
{
	NULL_TERMINATED_BYTES_SOURCE = 1
}
Write_Bytes_Function;


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
	Number                 module_address;
	struct Writer_Context* next;

	void (*initialize)      (Writer* writer, Byte* source, Integer_Number (*write_bytes)(Byte* source, Byte* bytes, Number size_of_bytes));
	void (*deinitialize)    (Writer* writer);
	void (*flush)           (Writer* writer);
	void (*print)           (Writer* writer, Byte* parameters, Byte** arguments);
	void (*print_in_string) (Byte* string, Number max_size_of_string, Byte* parameters, ...);
}
Writer_Context;