//ven_8086 dev_1237 - Intel Corporation 440FX - 82441FX PMC (chipset)
//ven_8086 dev_7000 - Intel Corporation 82371SB PIIX3 ISA
//ven_8086 dev_7111 - Intel Corporation 82371AB/EB/MB PIIX4 IDE
//ven_80EE dev_BEEF - VirtualBox Graphics Adapter
//ven_1022 dev_2000 - AMD PCNET
//ven_80EE dev_CAFE - VirtualBox Guest Service
//ven_8086 dev_2415 - AC'97 Audio
//ven_106B dev_003F - Apple Inc. KeyLargo/Intrepid USB
//ven_8086 dev_7113 - Intel 82371AB Power Management Controller

//#define input(name) void name(); asm(".asciz \"" #name "\"");
//input(abc)


void main();
void kernel_main()
{
	main();
}


#include <types.c>
#include "video/VESA.c"
#include "text-screen.c"
#include "interrupts.c"
#include "errors.c"
#include "keyboard.c"
#include "timer.c"
//#include "PCI.c"

#include "disk/ATA-PIO.c"

#include "USB/UHCI.c"
//#include "USB/OHCI.c"
#include "USB/EHCI.c"


//#include <program/executor.c>


//#include "FAT.c"


//#include "commander.c"


//#include "UI.c"


/*
#include "audio/AC97.c"


void test_PCI_device(PCI_Device* device)
{
	if(device->config.vendor = 0x8086 && device->config.device == 0x2415)
		test_AC97(device);
}
*/


#include "../../PCI/1022_2000.c"


typedef enum
{
	IDE_PRIMARY_IN_PCI_NATIVE_MODE   = 1,
	IDE_PRIMARY_MODE_IS_EDITABLE     = 2,
	IDE_SECONDARY_IN_PCI_NATIVE_MODE = 1,
	IDE_SECONDARY_MODE_IS_EDITABLE   = 2,
	IDE_SUPPORT_DMA                  = 128
}
IDE_PROGRAM_INTERFACE;


void configure_PCI_device(PCI_Device* device)
{
	/*
	if(device->config.class == 12 && device->config.subclass == 3 && device->config.prog_IF == 0)//USB UHCI
	{
		print("YFQLTY .C, E[CB\n");
		configure_USB_UHCI_device(device);
	}*/
	
	/*
	if(device->config.class == 12 && device->config.subclass == 3 && device->config.prog_IF == 16)//USB OHCI
	{
		print("YFQLTY .C, J[CB\n");
		print_PCI_device(device);
	}*/
	
	/*
	if(device->config.class == 12 && device->config.subclass == 3 && device->config.prog_IF == 32)//USB EHCI
	{
		print("YFQLTY .C, T[CB\n");
		configure_USB_EHCI_device(device);
	}*/
	
	/*
	if(device->config.class == 12 && device->config.subclass == 3 && device->config.prog_IF == 48)//USB XHCI
	{
		print("YFQLTY .C, BRC[CB\n");
		print_PCI_device(device);
	}*/
	
	/*
	if(device->config.class == 1 && device->config.subclass == 1) //IDE
	{
		print("YFQLTY GFHFKKTKMYSQ BYNTHATQC LBCRJD\n");

		if(device->config.prog_IF & IDE_SUPPORT_DMA)
			print("GHZVFZ PFGBCM GJLLTH;BDFTNCZ\n");//прямая запись поддерживается

		print(
			"FLHTC0 ", _Number, device->config.BARs[0], "\n",
			"FLHTC1 ", _Number, device->config.BARs[1], "\n",
			"FLHTC2 ", _Number, device->config.BARs[2], "\n",
			"FLHTC3 ", _Number, device->config.BARs[3], "\n",
			"FLHTC4 ", _Number, device->config.BARs[4], "\n"
		);
	}*/

	
	if(device->config.vendor == 0x1022 && device->config.device == 0x2000) //AMD PCNET
	{
		print("YFQLTY CTNTDJQ RJYNHJKKTH FVL\n");

		print_PCI_device(device);
		configure_AMD_PCNET(device);
	}
	
	/*
	if(device->config.vendor == 0x8086 && device->config.device == 0x100E) //Intel gigabit Ethernet
	{
		print("YFQLTY CTNTDJQ RJYNHJKKTH BYNTK\n");

		print_PCI_device(device);
	}*/
	
	
	if(device->config.vendor == 0x10EC && device->config.device == 0x8139) //Realtek 8139 Ethernet
	{
		print("YFQLTY CTNTDJQ RJYNHJKKTH HTFKNTR 8139\n");
		print_PCI_device(device);

		Number16 pio = get_PCI_IO_address(device, 1);
	}
	
	
	if(device->config.vendor == 0x10EC && device->config.device == 0x8168) //Realtek 8168 Ethernet
	{
		print("YFQLTY CTNTDJQ RJYNHJKKTH HTFKNTR 8168\n");
		print_PCI_device(device);
	}

	//print_PCI_device(device);
}

void configure_PCI_devices()
{
	find_PCI_devices(&configure_PCI_device);
}


void main()
{
	initialize_default_text_screen();
/*
	VESA_Mode_Info* video_mode = 0x7D00;
	print(_Number, video_mode->framebuffer, " ", _Number, video_mode->pitch, " ", _Number, video_mode->width, " ", _Number, video_mode->height);

	draw_VESA_rectangle(video_mode, 0, 668, 255, 100, 255 * 256);*/

	clear_text_screen();
	initialize_interrupts();
	initialize_timer();
	add_division_by_zero_interrupt_handler();
	initialize_keyboard();

	//hide_text_screen_cursor();

	//test_VESA();

	//print(0x2000);
	//print("Hi ", _Number, 1234, 0);
	//find_PCI_devices(&print_PCI_device);
	//find_PCI_devices(&test_PCI_device);

	//Byte* bytes = 1024 * 1024;
	//read_bytes_from_ATA_PIO(0, bytes, 130560);

	//print(&write_hex_Number32, *(Number32*)bytes);
	//print(&write_Number, sizeof(Number64));

	//configure_USB_OHCI_devices();

	//emain();


/*
	FAT_File_System file_system;

	open_FAT_File_System(&file_system, 0, &read_ATA_PIO_sector, &write_ATA_PIO_sector);
	//create_FAT_directory(&file_system, L"POPKA");
	//list_FAT_directory(&file_system);

	FAT_Data file;
	open_FAT_file(&file_system, L"DRIVER", &file);
	
	Byte sector[513];
	clear_bytes(sector, 513);
	read_FAT_sector(&file_system, file.cluster_number_low + (file.cluster_number_high << 16), sector, 0);
	print(sector);


	FAT_Data file;
	open_FAT_file(&file_system, L"USB", &file);
	
	Byte sector[513];
	clear_bytes(sector, 513);
	read_FAT_sector(&file_system, file.cluster_number_low + (file.cluster_number_high << 16), sector, 0);

	execute(sector, 1024 * 1024, 2 * 1024 * 1024);

	//print(_Number, *(Number*)(1024 * 1024));
	*/

	//draw_program();
	//start_commander();

	//Number i;
	//for(i = 1; i < 256; ++i)
	//	print("(", &i, " ", _Number, i, ") ");

	//Number i;
	//for(i = 0; i < 10; ++i)
	//	((Byte*)0xB8000)[i] = 25;

	configure_PCI_devices();
}

//#include "snake.c"