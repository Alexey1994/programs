#ifndef PCI_INCLUDED
#define PCI_INCLUDED


typedef struct
{
	Number16 ID;

	struct {
		Number16 vendor;
		Number16 device;
		Number16 command;
		Number16 status;
		Number8  revision;
		Number8  prog_IF;
		Number8  subclass;
		Number8  class;
		Number8  cache_line_size;
		Number8  latency_timer;
		Number8  header_type;
		Number8  BIST;
		Number32 BARs[6];
		Number32 cardbus_CIS_pointer;
		Number16 subsystem_vendor;
		Number16 subsystem;
		Number32 expansion_ROM_base_address;
		Number8  capabilities_pointer;
		Number8  reserved1;
		Number16 reserved2;
		Number32 reserved3;
		Number8  interrupt_line;
		Number8  interrupt_pin;
		Number8  min_grant;
		Number8  max_latency;
	}
	config;
}
PCI_Device;


typedef enum
{
	PCI_VENDOR_ID           = 0,
	PCI_DEVICE_ID           = 2,
	PCI_COMMAND             = 4,
	PCI_STATUS              = 6,
	PCI_REVISION_ID         = 8,
	PCI_PROG_IF             = 9,
	PCI_SUBCLASS            = 10,
	PCI_CLASS_CODE          = 11,
	PCI_CACHE_LINE_SIZE     = 12,
	PCI_LATENCY_TIMER       = 13,
	PCI_HEADER_TYPE         = 14,
	PCI_BIST                = 15,
	PCI_BAR0                = 16,
	PCI_BAR1                = 20,
	PCI_BAR2                = 24,
	PCI_BAR3                = 28,
	PCI_BAR4                = 32,
	PCI_BAR5                  = 36,
	PCI_CARDBUS_CIS_POINTER   = 40,
	PCI_SUBSYSTEM_VENDOR_ID   = 44,
	PCI_SUBSYSTEM_ID          = 46,
	PCI_EXPANSION_ROM_ADDRESS = 48,
	PCI_CAPABILITIES_POINTER  = 52,
	PCI_INTERRUPT_LINE        = 60,
	PCI_INTERRUPT_PIN         = 61,
	PCI_MIN_GRANT             = 62,
	PCI_MAX_LATENCY           = 63
}
PCI_Field;


/*
   | bits 31-24  | bits 23-16  |  bits 15-8    | bits 7-0             |
-----------------------------------------------------------------------
00 | Device ID                 |  Vendor ID                           |
   --------------------------------------------------------------------
04 | Status                    |  Command                             |
   --------------------------------------------------------------------
08 | Class code  | Subclass    |  Prog IF      | Revision ID          |
   --------------------------------------------------------------------
0C | BIST        | Header type | Latency Timer | Cache Line Size      |
   --------------------------------------------------------------------
10 | Base address #0 (BAR0)                                           |
   --------------------------------------------------------------------
14 | Base address #1 (BAR1)                                           |
   --------------------------------------------------------------------
18 | Base address #2 (BAR2)                                           |
   --------------------------------------------------------------------
1C | Base address #3 (BAR3)                                           |
   --------------------------------------------------------------------
20 | Base address #4 (BAR4)                                           |
   --------------------------------------------------------------------
24 | Base address #5 (BAR5)                                           |
   --------------------------------------------------------------------
28 | Cardbus CIS Pointer                                              |
   --------------------------------------------------------------------
2C | Subsystem ID              | Subsystem Vendor ID                  |
   --------------------------------------------------------------------
30 | Expansion ROM base address                                       |
   --------------------------------------------------------------------
34 | Reserved                                  | Capabilities Pointer |
   --------------------------------------------------------------------
38 | Reserved                                                         |
   --------------------------------------------------------------------
3C | Max latency | Min Grant   | Interrupt PIN | Interrupt Line       |
   -------------------------------------------------------------------- 
*/


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


void find_PCI_devices(void (on_finded)(PCI_Device* device))
{
	PCI_Device device;

	for(device.ID = 0; ; )
	{
		*(Number32*)&device.config = read_from_PCI_device_config(&device, 0);

		if(device.config.vendor != 0xFFFF && device.config.device != 0xFFFF)
		{
			Number i;
			for(i = 1; i < 16; ++i)
				((Number32*)&device.config)[i] = read_from_PCI_device_config(&device, i * 4);

			on_finded(&device);
		}

		++device.ID;

		if(!device.ID)
			break;
	}
}


Number32 get_PCI_MMIO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] & 0b11111111111111111111111111110000;
}


Number32 get_PCI_IO_address(PCI_Device* device, Number BAR_number)
{
	return device->config.BARs[BAR_number] >> 2;
}


void print_PCI_device(PCI_Device* device)
{
	Number i;

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
		write_in_PCI_device_config(device, PCI_BAR0 + i * 4, 0xFFFFFFFF);
		Number space_required = -read_from_PCI_device_config(device, PCI_BAR0 + i * 4);
		write_in_PCI_device_config(device, PCI_BAR0 + i * 4, bar_data);

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

	print("\n");
}


#endif//PCI_INCLUDED