#ifndef BYTES_WRITER_INCLUDED
#define BYTES_WRITER_INCLUDED


#include <writer.c>
#include <system/memory.c>
#include <memory.c>


typedef struct
{
	Byte*  bytes;
	Number size_of_bytes;
}
Bytes_Writer_Source;


Integer_Number32 write_bytes_in_Bytes_Writer(Bytes_Writer_Source* source, Byte* bytes, Number32 size_of_bytes)
{
	Integer_Number32 bytes_writed;

	if(source->size_of_bytes < size_of_bytes)
	{
		copy_bytes(source->bytes, bytes, source->size_of_bytes);
		bytes_writed = source->size_of_bytes;
		source->size_of_bytes = 0;
	}
	else
	{
		copy_bytes(source->bytes, bytes, size_of_bytes);
		bytes_writed = size_of_bytes;
		source->bytes += size_of_bytes;
		source->size_of_bytes -= size_of_bytes;
	}

	return bytes_writed;
}


void initialize_bytes_writer(Writer* writer, Byte* bytes, Number size_of_bytes)
{
	Bytes_Writer_Source* source;

	source = allocate_memory(sizeof(Bytes_Writer_Source));
	source->bytes         = bytes;
	source->size_of_bytes = size_of_bytes;
	
	initialize_writer(writer, source, &write_bytes_in_Bytes_Writer);
	writer->close_source = &free_memory;
}


#endif //BYTES_WRITER_INCLUDED

/*
void main()
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

	log(bytes, "\n");
}*/