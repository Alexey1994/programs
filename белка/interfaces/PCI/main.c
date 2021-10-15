#include <interfaces/PCI.h>
#undef print


Number32 read_from_PCI_device_config(Kernel* kernel, PCI_Device* device, PCI_Field field_address);
void write_in_PCI_device_config(Kernel* kernel, PCI_Device* device, PCI_Field field_address, Number32 data);

Number32 get_PCI_MMIO_address(PCI_Device* device, Number BAR_number);
Number32 get_PCI_IO_address(PCI_Device* device, Number BAR_number);

void print_PCI_device(Kernel* kernel, PCI_Device* device);

PCI_Driver load_PCI_driver(PCI* pci, PCI_Device* device);


void enum_PCI_devices(Kernel* kernel, Number module_address)
{
	//Byte message[] = {'G', 'H', 'B', 'D', 0};
	//kernel->_print(kernel->screen, kernel->_Number, 1234, message, 0);
	//kernel->end_print(kernel->screen);

	PCI pci = {
		.kernel = kernel,

		.read_from_PCI_device_config = &read_from_PCI_device_config + module_address,
		.write_in_PCI_device_config  = &write_in_PCI_device_config + module_address,

		.get_PCI_MMIO_address = &get_PCI_MMIO_address + module_address,
		.get_PCI_IO_address   = &get_PCI_IO_address + module_address,

		.print_PCI_device = &print_PCI_device + module_address
	};

	PCI_Device device;
	PCI_Driver driver;

	for(device.ID = 0; ; )
	{
		*(Number32*)&device.config = read_from_PCI_device_config(kernel, &device, 0);

		if(device.config.vendor != 0xFFFF && device.config.device != 0xFFFF)
		{
			Number i;
			for(i = 1; i < 16; ++i)
				((Number32*)&device.config)[i] = read_from_PCI_device_config(kernel, &device, i * 4);

			driver = load_PCI_driver(&pci, &device);

			if(driver)
			{
				//kernel->_print(kernel->screen, kernel->_Number, 1234, 0);
				//kernel->end_print(kernel->screen);

				//if(device.config.vendor == 0x106B)
				if(device.config.device == 0x1E31)
					driver(&pci, &device, driver);
			}

			//on_finded(&device);
			//print_PCI_device(kernel, &device);


			//kernel->_print(kernel->screen, name, 0);
			//kernel->end_print(kernel->screen);
		}

		++device.ID;

		if(!device.ID)
			break;
	}
}


Byte get_hex_character(Byte byte)
{
	if(byte < 10)
		return byte + '0';
	else
		return byte - 10 + 'A';
}

void print_PCI_device(Kernel* kernel, PCI_Device* device);
PCI_Driver load_PCI_driver(PCI* pci, PCI_Device* device)
{
	//print_PCI_device(pci->kernel, device);

	Number16 name[] = {
		get_hex_character((device->config.vendor >> 12) & 0b1111),
		get_hex_character((device->config.vendor >> 8) & 0b1111),
		get_hex_character((device->config.vendor >> 4) & 0b1111),
		get_hex_character((device->config.vendor >> 0) & 0b1111),

		get_hex_character((device->config.device >> 12) & 0b1111),
		get_hex_character((device->config.device >> 8) & 0b1111),
		get_hex_character((device->config.device >> 4) & 0b1111),
		get_hex_character((device->config.device >> 0) & 0b1111),

		'\0'
	};

	Number16 pci_directory[] = {
		'P',
		'C',
		'I',
		'\0'
	};

	return pci->kernel->load_module(pci_directory, name);
}


void set_PCI_device_config_address(Kernel* kernel, PCI_Device* device, PCI_Field field_address)
{
	kernel->out_32(
		0xCF8,
		0b10000000000000000000000000000000 //enable bit
		| (device->ID << 8)
		| field_address
	);
}


Number32 read_from_PCI_device_config(Kernel* kernel, PCI_Device* device, PCI_Field field_address)
{
	set_PCI_device_config_address(kernel, device, field_address);
	return kernel->in_32(0xCFC);
}


void write_in_PCI_device_config(Kernel* kernel, PCI_Device* device, PCI_Field field_address, Number32 data)
{
	set_PCI_device_config_address(kernel, device, field_address);
	kernel->out_32(0xCFC, data);
}


Number32 get_PCI_MMIO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] & 0b11111111111111111111111111110000;
}


Number32 get_PCI_IO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] >> 2;
}


void print_PCI_device(Kernel* kernel, PCI_Device* device)
{
	Number i;
/*
	print(
		"GHJBPDJLBNTKM ", _hex_Number16, device->config.vendor,
		" ECNHJQCNDJ ", _hex_Number16, device->config.device,
		" HTDBPBZ ", _Number, device->config.revision,
		" RKFCC ", _Number, device->config.class,
		"\n"
	);

	for(i = 0; i < 6; ++i)
	{
		Number32 bar_data = device->config.BARs[i];
		write_in_PCI_device_config(kernel, device, PCI_BAR0 + i * 4, 0xFFFFFFFF);
		Number space_required = -read_from_PCI_device_config(kernel, device, PCI_BAR0 + i * 4);
		write_in_PCI_device_config(kernel, device, PCI_BAR0 + i * 4, bar_data);

		if(!device->config.BARs[i])
			continue;

		if(device->config.BARs[i] & 1)
		{
			print(
				"GJHN ", //порт
				_Number, get_PCI_IO_address(device, i),
				" PFYBVFTN ", //занимает
				_Number, space_required,
				" ,fqn\n" //байт
			);
		}
		else
		{
			Byte type;

			type = (device->config.BARs[i] << 1) & 0b11;

			if(type == 0)
			{
				print(
					"GFVZNM32 ", //память32
					_Number, get_PCI_MMIO_address(device, i),
					" PFYBVFTN ", //занимает
					_Number, space_required,
					" ,fqn\n" //байт
				);
			}
			if(type == 1)
			{
				print(
					"GFVZNM16 ", //память16
					_Number, get_PCI_MMIO_address(device, i),
					"\n"
				);
			}
			else if(type == 2)
			{
				print(
					"GFVZNM64 ", //память64
					_Number, get_PCI_MMIO_address(device, i),
					"\n"
				);
			}
		}
	}

	print("\n");*/

	Byte message[] = {'h', 'c', 'h', 'c', 0};
	kernel->print(message, device->config.vendor, ' ', device->config.device, '\n');
/*
	for(i = 0; i < 6; ++i)
	{
		Number32 bar_data = device->config.BARs[i];
		write_in_PCI_device_config(kernel, device, PCI_BAR0 + i * 4, 0xFFFFFFFF);
		Number space_required = -read_from_PCI_device_config(kernel, device, PCI_BAR0 + i * 4);
		write_in_PCI_device_config(kernel, device, PCI_BAR0 + i * 4, bar_data);

		if(space_required)
		{
			Byte message[] = {'n', 'c', 'n', 'c', 0};
			kernel->print(message, i, ' ', space_required, '\n');
		}
	}*/
}

//8086 1237 - Intel Corporation 440FX - 82441FX PMC (chipset)
//8086 7000 - Intel Corporation 82371SB PIIX3 ISA
//8086 7010 - Intel Corporation 82371SB PIIX3 IDE
//8086 7020 - Intel Corporation 82371SB PIIX3 USB
//8086 7113 - Intel Corporation 82371AB/EB/MB PIIX4 ACPI
//1234 1111 - Qemu virtual video
//8086 100E - 82540EMmGigabit Ethernet Controller
//8086 2415 - AC'97 Audio
//106B 003F - KeyLargo/Interpid USB
//8086 24CD - 82801DB/DBM (ICH4/ICH4-M) USB2 EHCI
//1B36 000D - Qemu XHCI Controller
//1022 2000 - PCnet Ethernet
//8086 2922 - 82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA AHCI Controller
//1002 5046 - ATI Rage 128 PRO AGP 4X
