#include <file.c>
#include <storage/MBR.c>
#include <storage/FAT.c>


Boolean append_FAT_file(FAT_File_System* file_system, Number16* file_path, Number16* new_file_name)
{
	File    appended_file;
	Number  appended_file_size;
	Boolean is_success = 1;

	appended_file = open_file(file_path);
	if(!appended_file)
	{
		log_error(_null_terminated_words, file_path, " not found");
		goto error;
	}

	appended_file_size = get_file_size(appended_file);

	Byte* appended_file_data = allocate_memory(appended_file_size);
	read_from_file(appended_file, 0, appended_file_data, appended_file_size);
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


	if(!append_FAT_file(&file_system, L"kernel/bin/kernel", L"KERNEL"))
		goto error;

	if(!create_FAT_directory(&file_system, L"IFACES"))
		goto error;

	if(!open_FAT_directory(&file_system, L"IFACES"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/bin/PCI", L"PCI"))
		goto error;


	if(!open_FAT_File_System(&file_system, storage_file))
		goto error;

	if(!create_FAT_directory(&file_system, L"PCI"))
		goto error;

	if(!open_FAT_directory(&file_system, L"PCI"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/10025046/bin/10025046", L"10025046"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/10222000/bin/10222000", L"10222000"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/10EC8168/bin/10EC8168", L"10EC8168"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/12341111/bin/12341111", L"12341111"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/13445410/bin/13445410", L"13445410"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80862415/bin/80862415", L"80862415"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/8086280B/bin/8086280B", L"8086280B"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80863EA0/bin/80863EA0", L"80863EA0"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80867010/bin/80867010", L"80867010"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80867111/bin/80867111", L"80867111"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80869DD3/bin/80869DD3", L"80869DD3"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80869DED/bin/80869DED", L"80869DED"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80869DF0/bin/80869DF0", L"80869DF0"))
		goto error;

	if(!append_FAT_file(&file_system, L"PCI/devices/80869DF5/bin/80869DF5", L"80869DF5"))
		goto error;
/*
	if(!append_FAT_file(&file_system, L"./doc/man.txt", L"GHBDTN"))
		goto error;

	if(!create_FAT_directory(&file_system, L"BYNTHATQCS"))//¨­â¥àä¥©áë
		goto error;

	if(!create_FAT_directory(&file_system, L"ECNHJQCNDF"))//ãáâà®©áâ¢ 
		goto error;

	if(!open_FAT_directory(&file_system, L"BYNTHATQCS"))
		goto error;

	if(!append_FAT_file(&file_system, L"./drivers/PCI", L"GRB"))//Šˆ, PCI
		goto error;*/


	//if(!append_FAT_file(&file_system, L"./drivers/OUT", L"USB"))
	//	goto error;
/*
	if(!create_FAT_directory(&file_system, L"PCI"))
		goto error;

	if(!create_FAT_directory(&file_system, L"ACPI"))
		goto error;

	if(!create_FAT_directory(&file_system, L"USB"))
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

	delete_file(L"VM/storage");
	create_file(L"VM/storage", 0, 0);
	storage_file = open_file(L"VM/storage");
	for(i = 0; i < 4096; ++i)
		write_in_file(storage_file, i * 512, zeros, 512);
	close_file(storage_file);
}


Number32 main()
{
	create_storage();

	File     storage_file;
	Number64 storage_file_size;

	storage_file = open_file(L"VM/storage");
	if(!storage_file)
	{
		log_error("can't open \"VM/storage\" file");
		return 1;
	}
	storage_file_size = get_file_size(storage_file);
	print("storage size: ", _Number64_triplets, storage_file_size, " bytes");

	create_partition(storage_file, storage_file_size);
	format_FAT32(storage_file, storage_file_size);
	append_FAT_files(storage_file);

	close_file(storage_file);

	return 0;
}
