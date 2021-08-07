void main();
void kernel_main()
{
	main();
}


#include <types.c>

//#include "VESA.c"
//#include "text-screen.c"

#include "text-screen-VGA.c"

#include "interrupts.c"
#include "keyboard.c"
#include "timer.c"

#include "ATA-PIO.c"
#include "FAT.c"


#include "kernel.h"


Boolean is_memory_cell(Byte* cell)
{
	*cell = 1;

	if(*cell != 1)
		return 0;

	*cell = 0;

	if(*cell != 0)
		return 0;

	return 1;
}

Number calculate_memory_size()
{
	Byte* left;
	Byte* right;
	Byte* center;

	left = 0;
	right = 0xFFFFFFFF;

	while((Number)right - (Number)left > 1)
	{
		center = (Number)left / 2 + (Number)right / 2;

		if(is_memory_cell(center))
			left = center;
		else
			right = center;
	}

	return right;
}


Module module_top = 1024 * 1024;

Module load_module(Number16* directory, Number16* path)
{
	Module          loaded_module;
	FAT_File_System file_system;

	open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);

	if(directory)
	{
		if(!open_FAT_directory(&file_system, directory))
			goto error;
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


static Boolean load_interface(FAT_File_System* file_system, FAT_Data* data, Number16* name, Number32 data_offset, Kernel* kernel)
{
	Module interface;

	interface = load_module(L"IFACES", name);

	if(interface)
		interface(kernel, interface);

	return 1;
}

Boolean load_interfaces(Kernel* kernel)
{
	FAT_File_System file_system;

	open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);
	
	if(!open_FAT_directory(&file_system, L"IFACES"))
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


//#include "pci-test.c"


void main()
{
	initialize_default_text_screen();
	clear_text_screen();

	initialize_interrupts();
	initialize_timer();
	initialize_keyboard();

	Kernel kernel = {
		.memory_size = calculate_memory_size(),

		.in_8   = &in_8,
		.out_8  = &out_8,
		.in_16  = &in_16,
		.out_16 = &out_16,
		.in_32  = &in_32,
		.out_32 = &out_32,

		.add_interrupt_handler = &add_interrupt_handler,

		.print  = &print,

		.load_module = &load_module,
	};

	load_interfaces(&kernel);
	//enum_PCI_devices();
}