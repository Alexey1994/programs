#ifndef FILE_INCLUDED
#define FILE_INCLUDED


#include <system/file.c>
#include <reader.c>
#include <writer.c>
#include <system/memory.c>


typedef struct
{
	File     file;
	Number32 position;
	Number64 size;
}
File_Source;


Number read_bytes_from_file (File_Source* source, Byte* bytes, Number number_of_bytes)
{
	Number bytes_readed;

	bytes_readed = read_from_file(source->file, source->position, bytes, number_of_bytes);
	source->position += bytes_readed;

	return bytes_readed;
}


Boolean end_of_file (File_Source* source)
{
	return source->position >= source->size;
}


Number write_bytes_in_file (File_Source* source, Byte* bytes, Number number_of_bytes)
{
	Number bytes_writed;

	bytes_writed = write_in_file(source->file, source->position, bytes, number_of_bytes);
	source->position += bytes_writed;

	return bytes_writed;
}


static void close_file_source (File_Source* source)
{
	close_file(source->file);
	free_memory(source);
}


void initialize_file_reader (Reader* reader, Bit16* file_path)
{
	File_Source* source;

	source = allocate_memory(sizeof(File_Source));
	source->file     = open_file(file_path);
	source->position = 0;
	source->size     = get_file_size(source->file);

	initialize_reader(reader, source, &read_bytes_from_file);
	reader->end_of_data  = &end_of_file;
	reader->close_source = &close_file_source;
}


void initialize_file_reader_from_file (Reader* reader, File file)
{
	File_Source* source;

	source = allocate_memory(sizeof(File_Source));
	source->file     = file;
	source->position = 0;
	source->size     = get_file_size(source->file);

	initialize_reader(reader, source, &read_bytes_from_file);
	reader->end_of_data  = &end_of_file;
	reader->close_source = &free_memory;
}


void initialize_file_writer (Writer* writer, Bit16* file_path)
{
	File_Source* source;

	create_file(file_path, 0, 0);
	source = allocate_memory(sizeof(File_Source));
	source->file     = open_file(file_path);
	source->position = 0;

	initialize_writer(writer, source, &write_bytes_in_file);
	writer->close_source = &close_file_source;
}


void set_file_position (Reader* reader, Number32 position)
{
	File_Source* source;

	source = reader->source;
	source->position = position;
	clear_buffer(&reader->buffer);
}


#define FILE_READER(file_path)\
{\
	Reader reader;\
	initialize_file_reader(&reader, (file_path));


#define FILE_WRITER(file_path)\
{\
	Writer writer;\
	initialize_file_writer(&writer, (file_path));


#endif//FILE_INCLUDED

/*
void main()
{
	FILE_READER(L"a.txt")
		printf("%d\n", read(Number));
		read(spaces);
		read(line);
		printf("%d\n", read(Number));
	END_READER

	FILE_WRITER(L"b.txt")
		print_in_writer(&writer, "tada", " dam", 0);
	END_WRITER
}*/