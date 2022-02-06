#ifndef WRITER_INCLUDED
#define WRITER_INCLUDED


typedef struct
{
	Kernel* kernel;
	Number  module_address;

	Byte* source;

	Integer_Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 size_of_bytes);
	void             (*close_source)(Byte* source);

	Number   buffer_used;
	Byte     buffer[OUTPUT_BUFFER_SIZE];
}
Writer_Module;


typedef struct
{
	Kernel* kernel;
	Number  module_address;

	Byte* source;

	Integer_Number32 (*write_bytes)(Byte* source, Byte* bytes, Number32 size_of_bytes);
	void             (*close_source)(Byte* source);

	Number   buffer_used;
	Byte     buffer[OUTPUT_BUFFER_SIZE];
}
Writer;


#endif//WRITER_INCLUDED