#include <file.c>


Number main()
{
	File   in_file;
	Number in_file_size;
	Number i;
	Byte   byte;

	in_file = open_file("bin/components/main-loader16");

	if(!in_file)
	{
		log_error("main-loader16 не найден");
		return 1;
	}

	in_file_size = get_file_size(in_file);

	Writer writer;
	initialize_file_writer(&writer, "bin/components/main-loader16.hex");
	{
		for(i = 0; i < in_file_size; ++i)
		{
			read_bytes_from_file(in_file, &byte, 1);

			write(null_terminated_bytes, "BYTE(");
			write(hex_Number8, byte);
			write(null_terminated_bytes, ") ");
		}
	}
	deinitialize_writer(&writer);

	close_file(in_file);

	return 0;
}