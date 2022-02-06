#include <file.c>
#include <storage/MBR.c>
#include <storage/FAT.c>


Boolean append_FAT_file(FAT_File_System* file_system, Byte* file_path, Number16* new_file_name)
{
	File    appended_file;
	Number  appended_file_size;
	Boolean is_success = 1;

	appended_file = open_file(file_path);
	if(!appended_file)
	{
		log_error(file_path, " не найден");
		goto error;
	}

	appended_file_size = get_file_size(appended_file);

	Byte* appended_file_data = allocate_memory(appended_file_size);
	read_bytes_from_file(appended_file, appended_file_data, appended_file_size);
	if(!create_FAT_file(file_system, new_file_name, appended_file_data, appended_file_size))
		is_success = 0;
	free_memory(appended_file_data);
	close_file(appended_file);

	return is_success;

error:
	return 0;
}


Boolean append_FAT_files(File storage_file)
{
	FAT_File_System file_system;

	if(!open_FAT_File_System(&file_system, storage_file))
		goto error;

	if(!append_FAT_file(&file_system, "bin/components/kernel", L"KERNEL"))
		goto error;

	if(!create_FAT_directory(&file_system, L"IFACES"))
		goto error;

	if(!open_FAT_directory(&file_system, L"IFACES"))
		goto error;


	if(!append_FAT_file(&file_system, "bin/components/interfaces/writer/out", L"WRITER"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/components/interfaces/console/out", L"CONSOLE"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/components/interfaces/display/out", L"DISPLAY"))
		goto error;


	if(!open_FAT_File_System(&file_system, storage_file))
		goto error;

	if(!create_FAT_directory(&file_system, L"DRIVERS"))
		goto error;

	if(!open_FAT_directory(&file_system, L"DRIVERS"))
		goto error;

	if(!create_FAT_directory(&file_system, L"DISPLAY"))
		goto error;

	if(!open_FAT_directory(&file_system, L"DISPLAY"))
		goto error;


	if(!append_FAT_file(&file_system, "bin/components/drivers/display/VGA/out", L"VGA"))
		goto error;


	/*
	if(!append_FAT_file(&file_system, "bin/interfaces/video/out", L"VIDEO"))
		goto error;


	if(!append_FAT_file(&file_system, "bin/interfaces/PCI/out", L"PCI"))
		goto error;

	if(!open_FAT_File_System(&file_system, storage_file))
		goto error;

	if(!create_FAT_directory(&file_system, L"PCI"))
		goto error;

	if(!open_FAT_directory(&file_system, L"PCI"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/10025046/out", L"10025046"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/10222000/out", L"10222000"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/106B003F/out", L"106B003F"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/10EC8168/out", L"10EC8168"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/12341111/out", L"12341111"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/13445410/out", L"13445410"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80861E31/out", L"80861E31"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80862415/out", L"80862415"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/8086280B/out", L"8086280B"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80863EA0/out", L"80863EA0"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80867010/out", L"80867010"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80867111/out", L"80867111"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80869DD3/out", L"80869DD3"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80869DED/out", L"80869DED"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80869DF0/out", L"80869DF0"))
		goto error;

	if(!append_FAT_file(&file_system, "bin/PCI/80869DF5/out", L"80869DF5"))
		goto error;*/

	return 1;

error:
	return 0;
}


void create_storage()
{
	Byte zeros[512] = {0};
	File storage_file;
	Number i;

	delete_file("bin/storage");
	storage_file = create_file("bin/storage");
	for(i = 0; i < 4096; ++i)
		write_bytes_in_file(storage_file, zeros, 512);
	close_file(storage_file);
}


Number main()
{
	create_storage();

	File   storage_file;
	Number storage_file_size;

	storage_file = open_file("bin/storage");
	if(!storage_file)
	{
		log_error("отсутствует файл \"bin/storage\"");
		return 1;
	}
	storage_file_size = get_file_size(storage_file);
	print("размер bin/storage: %d байт\n", storage_file_size);

	create_partition(storage_file, storage_file_size);
	format_FAT32(storage_file, storage_file_size);
	append_FAT_files(storage_file);

	close_file(storage_file);

	return 0;
}
