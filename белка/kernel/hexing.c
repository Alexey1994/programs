#include <file.c>


Number main()
{
	File   in_file;
	Number in_file_size;
	Number i;
	Byte   byte;

	in_file = open_file(L"bin/kernel16");

	if(!in_file)
	{
		log_error("kernel16 file not found");
		return 1;
	}

	in_file_size = get_file_size(in_file);

	FILE_WRITER(L"bin/kernel16.hex")
	{
		for(i = 0; i < in_file_size; ++i)
		{
			read_from_file(in_file, i, &byte, 1);

			write(null_terminated_bytes, "BYTE(");
			write(hex_Number8, byte);
			write(null_terminated_bytes, ") ");
		}
	}
	END_WRITER

	return 0;
}