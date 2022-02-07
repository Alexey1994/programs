void main();
void _start()
{
	main();
}


#include <types.c>

#include "ATA-PIO.c"
#include "FAT.c"

#include <kernel.h>


Module module_top = 1024 * 1024;


Byte* allocate_global_memory(Number size)
{
	Byte* allocated_memory;

	allocated_memory = module_top;
	module_top += size;

	return allocated_memory;
}


Module load_module(Byte* directory, Byte* path)
{
	Module          loaded_module;
	FAT_File_System file_system;
	Number          i;

	open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);

	if(directory)
	{
		Byte subdirectory[12];

		for(;;)
		{
			i = 0;
			while(*directory && *directory != '/')
			{
				subdirectory[i] = *directory;
				++directory;
				++i;
			}
			subdirectory[i] = '\0';

			if(!open_FAT_directory(&file_system, subdirectory))
				goto error;

			if(!*directory)
				break;

			++directory;
		}
	}

	FAT_Data file;
	if(!open_FAT_file(&file_system, path, &file))
		goto error;

	loaded_module = module_top;

	Number32 cluster_number;

	cluster_number = file.cluster_number_low + (file.cluster_number_high << 16);
	while(cluster_number != 0x0FFFFFFF)
	{
		read_FAT_sector(&file_system, cluster_number, module_top, 0);
		module_top += 512;
		cluster_number = next_FAT_cluster(&file_system, cluster_number);
	}

	return loaded_module;

error:
	return 0;
}


static Boolean load_interface(FAT_File_System* file_system, FAT_Data* data, Byte* name, Number32 data_offset, Kernel* kernel)
{
	Module interface;

	interface = load_module("IFACES", name);

	if(interface)
		interface(kernel, interface);

	return 1;
}

Boolean load_interfaces(Kernel* kernel)
{
	FAT_File_System file_system;

	open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);
	
	if(!open_FAT_directory(&file_system, "IFACES"))
		goto error;

	enum_FAT_data(
		&file_system,
		file_system.current_directory.cluster_number_low + (file_system.current_directory.cluster_number_high << 16),
		&load_interface,
		kernel
	);

	return 1;

error:
	return 0;
}


void main()
{
	Kernel kernel;

	clear_bytes(&kernel, sizeof(kernel));
	kernel.allocate_global_memory = &allocate_global_memory;
	kernel.load_module = &load_module;

	load_interfaces(&kernel);

	kernel.console->print(&kernel, "Modules loaded. %d bytes used\n", module_top - 1024 * 1024);
}