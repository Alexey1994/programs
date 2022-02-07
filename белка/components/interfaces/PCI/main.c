#include <kernel.h>


Number32 read_from_PCI_device_config(PCI_Device* device, PCI_Field field_address);
void write_in_PCI_device_config(PCI_Device* device, PCI_Field field_address, Number32 data);

Number32 get_PCI_MMIO_address(PCI_Device* device, Number BAR_number);
Number32 get_PCI_IO_address(PCI_Device* device, Number BAR_number);

PCI_Driver load_PCI_driver(Kernel* kernel, PCI_Device* device);


void start_module(Kernel* kernel, Number module_address)
{
	PCI_Context* pci;
	PCI_Device   device;
	PCI_Driver   driver;

	pci = kernel->pci = kernel->allocate_global_memory(sizeof(PCI_Context));
	pci->module_address = module_address;

	for(device.ID = 0; ; )
	{
		*(Number32*)&device.config = read_from_PCI_device_config(&device, 0);

		if(device.config.vendor != 0xFFFF && device.config.device != 0xFFFF)
		{
			Number i;
			for(i = 1; i < 16; ++i)
				((Number32*)&device.config)[i] = read_from_PCI_device_config(&device, i * 4);

			driver = load_PCI_driver(kernel, &device);

			if(driver)
			{
				//if(device.config.vendor == 0x106B)
				//if(device.config.device == 0x1E31)
				driver(kernel, driver, &device);
			}

			//kernel->console->print(kernel, module_address + "DEV_%x%x VEN_%x%x\n", device.config.vendor >> 8, device.config.vendor, device.config.device >> 8, device.config.device);
		}

		++device.ID;

		if(!device.ID)
			break;
	}
}


#include <IO.c>


PCI_Driver load_PCI_driver(Kernel* kernel, PCI_Device* device)
{
	Byte name[9];

	kernel->writer->print_in_string(
		name, 9, kernel->pci->module_address + "%x%x%x%x",

		(device->config.vendor >> 8) & 0xff,
		(device->config.vendor) & 0xff,

		(device->config.device >> 8) & 0xff,
		(device->config.device) & 0xff
	);

	return kernel->load_module(kernel->pci->module_address + "DRIVERS/PCI", name);
}


void set_PCI_device_config_address(PCI_Device* device, PCI_Field field_address)
{
	out_32(
		0xCF8,
		0b10000000000000000000000000000000 //enable bit
		| (device->ID << 8)
		| field_address
	);
}


Number32 read_from_PCI_device_config(PCI_Device* device, PCI_Field field_address)
{
	set_PCI_device_config_address(device, field_address);
	return in_32(0xCFC);
}


void write_in_PCI_device_config(PCI_Device* device, PCI_Field field_address, Number32 data)
{
	set_PCI_device_config_address(device, field_address);
	out_32(0xCFC, data);
}


Number32 get_PCI_MMIO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] & 0b11111111111111111111111111110000;
}


Number32 get_PCI_IO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] >> 2;
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
