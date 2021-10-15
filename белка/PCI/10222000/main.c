#include <types.c>
#undef print
#include <interfaces/PCI.h>


/*
|----------------------------------------------------------|
| 16 бит | 32 бита | режим работы                          |
|----------------------------------------------------------|
| 0                | 16 MAC адрес                          |
|----------------------------------------------------------|
| 0x10   | 0x10    | RDP - регистр данных для CSR          |
|----------------------------------------------------------|
| 0x12   | 0x14    | RAP - индексный регистр для CSR и BCR |
|----------------------------------------------------------|
| 0x14   | 0x18    | регистр сброса                        |
|----------------------------------------------------------|
| 0x16   | 0x1C    | BDP - регистр данных для BCR          |
|----------------------------------------------------------|

Адреса регистров в разных режимах работы устройства(16 бит и 32 бита).
*/


typedef struct
{
	Number32 packet_address;
	Number16 data_length; //непонятно как считается. The BCNT fields of the transmit and receive descriptors are 12-bit negative numbers representing the twos complement of the buffer size in bytes
	Number8  status1;
	Number8  status2;
	Number32 flags;
	Number32 reserved;
}
Receive_Transmit_Descriptor;


typedef struct
{
    Number16                      mode;
    Number8                       receive_buffer_count;  //степень 2-ки, максимум 9 (512 буферов)
    Number8                       transmit_buffer_count; //степень 2-ки, максимум 9 (512 буферов)
    Byte                          MAC_address[6];
    Number16                      reserved;
    Byte                          filter[8];
    Receive_Transmit_Descriptor  *receive_buffer_address;
    Receive_Transmit_Descriptor  *transmit_buffer_address;
}
Packet_Config;


void     set_register_index(PCI* pci, Number32 io, Number32 register_index);
Number32 read_from_CSR_register(PCI* pci, Number32 io, Number32 CSR_register_index);
void     write_in_CSR_register(PCI* pci, Number32 io, Number32 CSR_register_index, Number32 data);
Number32 read_from_BCR_register(PCI* pci, Number32 io, Number32 CSR_register_index);
void     write_in_BCR_register(PCI* pci, Number32 io, Number32 CSR_register_index, Number32 data);


void configure_AMD_PCNET(PCI* pci, PCI_Device* AMD_PCNET_device, Number module_address)
{
	Byte message[] = {'n', 'c', 0};
	//pci->kernel->print(message, 1022, '\n');
	//pci->kernel->print(module_address + "n", 1022);

	Number i;
	Number io;

	io = pci->get_PCI_IO_address(AMD_PCNET_device, 0);

	Byte* MAC_address = pci->get_PCI_MMIO_address(AMD_PCNET_device, 1);
	//print(MAC_address);
	//write_hex_array(get_default_text_screen_output(), MAC_address, 6);

	//reset
	pci->kernel->in_32(io + 0x18);
	pci->kernel->in_16(io + 0x14);

	//sleep 1ms
	for(i = 0; i < 1000000; ++i)
	{

	}

	//set 32 bit mode
	pci->kernel->out_32(io + 0x10, 0);

	//set SWSTYLE
	Number32 CSR_58 = read_from_CSR_register(pci, io, 58);
	write_in_CSR_register(pci, io, 58, CSR_58 & 0xfff0 | 2);

	//set ASEL (auto select 10 / 100baseT)
	Number32 BCR_2 = read_from_BCR_register(pci, io, 2);
	write_in_BCR_register(pci, io, 2, BCR_2 | 0b10);

	Receive_Transmit_Descriptor tdes[9];
	Receive_Transmit_Descriptor rdes[9];

	Byte buffer[1536];
	tdes->packet_address = buffer;

	Packet_Config config;

	config.mode = 0;
	config.receive_buffer_count = 9;
	config.transmit_buffer_count = 9;

	for(i = 0; i < 6; ++i)
		config.MAC_address[i] = MAC_address[i];

	for(i = 0; i < 8; ++i)
		config.filter[i] = 0;

	config.reserved = 0;
	config.receive_buffer_address = rdes;
	config.transmit_buffer_address = tdes;

	//configure card
	write_in_CSR_register(pci, io, 1, ((Number32)&config) & 0xffff);
	write_in_CSR_register(pci, io, 2, ((Number32)&config) >> 16);

	Number32 CSR_0;

	//initialize
	CSR_0 = read_from_CSR_register(pci, io, 0);
	write_in_CSR_register(pci, io, 0, CSR_0 | 1);

	while(!(read_from_CSR_register(pci, io, 0) & 0b10000000));

	//start
	CSR_0 = read_from_CSR_register(pci, io, 0);
	write_in_CSR_register(pci, io, 0, CSR_0 & 0b11111111111111111111111111111010 | 0b1010);

	buffer[0] = 'H';
	buffer[1] = 'i';

	//send packet
	tdes->status2 |= 0b11;
	tdes->data_length = -1536 & 0x0fff | 0xf000;
	tdes->status2 |= 0x80;
}


void set_register_index(PCI* pci, Number32 io, Number32 register_index)
{
	pci->kernel->out_32(io + 0x14, register_index);
}


Number32 read_from_CSR_register(PCI* pci, Number32 io, Number32 CSR_register_index)
{
	set_register_index(pci, io, CSR_register_index);
	return pci->kernel->in_32(io + 0x10);
}


void write_in_CSR_register(PCI* pci, Number32 io, Number32 CSR_register_index, Number32 data)
{
	set_register_index(pci, io, CSR_register_index);
	pci->kernel->out_32(io + 0x10, data);
}


Number32 read_from_BCR_register(PCI* pci, Number32 io, Number32 CSR_register_index)
{
	set_register_index(pci, io, CSR_register_index);
	return pci->kernel->in_32(io + 0x1c);
}


void write_in_BCR_register(PCI* pci, Number32 io, Number32 CSR_register_index, Number32 data)
{
	set_register_index(pci, io, CSR_register_index);
	pci->kernel->out_32(io + 0x1c, data);
}
