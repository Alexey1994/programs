#include <file.c>


Number main()
{
	Byte zeros[512] = {0};
	File storage_file;
	Number i;

	create_file(L"storage", 0, 0);
	storage_file = open_file(L"storage");
	for(i = 0; i < 4096; ++i)
		write_in_file(storage_file, i * 512, zeros, 512);
	close_file(storage_file);

	return 0;
}