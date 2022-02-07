#include "IO.c"


//https://lateblt.tripod.com/atapi.htm
//https://wiki.osdev.org/ATA_Command_Matrix
//https://wiki.osdev.org/ATA_PIO_Mode
//http://learnitonweb.com/2020/05/22/12-developing-an-operating-system-tutorial-episode-6-ata-pio-driver-osdev/


typedef enum
{
	ATA_STATUS_ERROR                   = 0b00000001,
	ATA_STATUS_INDEX                   = 0b00000010,
	ATA_STATUS_CORRECTED_DATA          = 0b00000100,
	ATA_STATUS_DATA_TRANSFER_REQUESTED = 0b00001000,
	ATA_STATUS_SERVICE_REQUEST         = 0b00010000,
	ATA_STATUS_DEVICE_FAULT            = 0b00100000,
	ATA_STATUS_READY                   = 0b01000000,
	ATA_STATUS_BUSY                    = 0b10000000
}
ATA_Status;


void read_ATA_PIO_sector(Number32 storage_number, Number32 start_sector, Number32* sector)
{
	Number i;

	while(in_8(0x1F7) & ATA_STATUS_BUSY);

	out_8(0x01F2, 1);                                //number of sectors
	out_8(0x01F3, start_sector);                     //bits 0-7 of LBA
	out_8(0x01F4, start_sector >> 8);                //bits 8-15 of LBA
	out_8(0x01F5, start_sector >> 16);               //bits 16-23 of LBA
	out_8(0x01F6, 0b11100000 | (start_sector >> 24)); //drive and bits 24-27 of LBA
	out_16(0x1F7, 0x20);                             //read sectors with retry

	while(in_8(0x1F7) & ATA_STATUS_BUSY);
	//while(!(in_8(0x1F7) & ATA_STATUS_READY));

	for(i = 0; i < 128; ++i)
		sector[i] = in_32(0x1F0);
}


void write_ATA_PIO_sector(Number32 storage_number, Number32 start_sector, Number32* sector)
{
	Number i;

	while(in_8(0x1F7) & ATA_STATUS_BUSY);

	out_8(0x01F2, 1);                                //number of sectors
	out_8(0x01F3, start_sector);                     //bits 0-7 of LBA
	out_8(0x01F4, start_sector >> 8);                //bits 8-15 of LBA
	out_8(0x01F5, start_sector >> 16);               //bits 16-23 of LBA
	out_8(0x01F6, 0b11100000 | (start_sector >> 24)); //drive and bits 24-27 of LBA
	out_16(0x1F7, 0x30);                             //write sectors with retry

	while(in_8(0x1F7) & ATA_STATUS_BUSY);
	//while(!(in_8(0x1F7) & ATA_STATUS_READY));

	for(i = 0; i < 128; ++i)
		out_32(0x1F0, sector[i]);
}


/*
void read_ATA_PIO_sectors(Number32 start_sector, Number32* sectors, Number number_of_sectors)
{
	Number i;
	Number j;

	while(in_8(0x1F7) & ATA_STATUS_BUSY);

	out_8(0x01F2, number_of_sectors);                //number of sectors
	out_8(0x01F3, start_sector);                     //bits 0-7 of LBA
	out_8(0x01F4, start_sector >> 8);                //bits 8-15 of LBA
	out_8(0x01F5, start_sector >> 16);               //bits 16-23 of LBA
	out_8(0x01F6, 11100000b | (start_sector >> 24)); //drive and bits 24-27 of LBA
	out_16(0x1F7, 0x20);                             //read sectors with retry

	for(i = 0; i < number_of_sectors; ++i)
	{
		while(in_8(0x1F7) & ATA_STATUS_BUSY);
		while(!(in_8(0x1F7) & ATA_STATUS_READY));

		for(j = 0; j < 128; ++j)
			sectors[j] = in_32(0x1F0);
	}
}


void read_bytes_from_ATA_PIO(Number32 offset, Byte* bytes, Number size_of_bytes)
{
	Byte   sector[512];
	Number start_sector;
	Number start_offset;
	Number i;

	start_sector = offset / 512;
	start_offset = offset % 512;

	if(start_offset)
	{
		read_ATA_PIO_sectors(start_sector, sector, 1);
		for(i = 0; i < (512 - start_offset) && size_of_bytes; ++i)
		{
			*bytes = sector[i + start_offset];
			++bytes;
			--size_of_bytes;
		}
		++start_sector;
	}

	while(size_of_bytes > 255 * 512)
	{
		read_ATA_PIO_sectors(start_sector, bytes, 255);
		size_of_bytes -= 255 * 512;
		start_sector += 255;
		bytes += 255 * 512;
	}

	while(size_of_bytes > 512)
	{
		read_ATA_PIO_sectors(start_sector, bytes, 1);
		size_of_bytes -= 512;
		++start_sector;
		bytes += 512;
	}

	if(size_of_bytes)
	{
		read_ATA_PIO_sectors(start_sector, sector, 1);
		for(i = 0; i < size_of_bytes; ++i)
		{
			*bytes = sector[i];
			++bytes;
		}
	}
}


void write_ATA_PIO_sectors(Number32 start_sector, Number32* sectors, Number number_of_sectors)
{
	Number i;
	Number j;

	while(in_8(0x1F7) & ATA_STATUS_BUSY);

	out_8(0x01F2, number_of_sectors);                //number of sectors
	out_8(0x01F3, start_sector);                     //bits 0-7 of LBA
	out_8(0x01F4, start_sector >> 8);                //bits 8-15 of LBA
	out_8(0x01F5, start_sector >> 16);               //bits 16-23 of LBA
	out_8(0x01F6, 11100000b | (start_sector >> 24)); //drive and bits 24-27 of LBA
	out_16(0x1F7, 0x30);                             //write sectors with retry

	for(i = 0; i < number_of_sectors; ++i)
	{
		while(in_8(0x1F7) & ATA_STATUS_BUSY);
		while(!(in_8(0x1F7) & ATA_STATUS_READY));

		for(j = 0; j < 128; ++j)
			out_32(0x1F0, sectors[j]);
	}
}


void write_bytes_in_ATA_PIO(Number32 offset, Byte* bytes, Number size_of_bytes)
{
	Byte   sector[512];
	Number start_sector;
	Number start_offset;
	Number i;

	start_sector = offset / 512;
	start_offset = offset % 512;

	if(start_offset)
	{
		read_ATA_PIO_sectors(start_sector, sector, 1);
		for(i = 0; i < 512 && i < size_of_bytes; ++i)
		{
			sector[i + start_offset] = *bytes;
			++bytes;
			--size_of_bytes;
		}
		write_ATA_PIO_sectors(start_sector, sector, 1);
		++start_sector;
	}

	while(size_of_bytes > 255 * 512)
	{
		write_ATA_PIO_sectors(start_sector, bytes, 255);
		size_of_bytes -= 255 * 512;
		start_sector += 255;
		bytes += 255 * 512;
	}

	while(size_of_bytes > 512)
	{
		write_ATA_PIO_sectors(start_sector, bytes, 1);
		size_of_bytes -= 512;
		++start_sector;
		bytes += 512;
	}

	if(size_of_bytes)
	{
		read_ATA_PIO_sectors(start_sector, sector, 1);
		for(i = 0; i < size_of_bytes; ++i)
		{
			sector[i] = *bytes;
			++bytes;
		}
		write_ATA_PIO_sectors(start_sector, sector, 1);
	}
}*/