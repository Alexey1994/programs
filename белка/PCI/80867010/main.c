#include <types.c>
#undef print
#include <interfaces/PCI.h>


typedef enum
{
	ATA_STATUS_ERROR               = 0x01,
	ATA_STATUS_INDEX               = 0x02,
	ATA_STATUS_CORRECTED_DATA      = 0x04,
	ATA_STATUS_DATA_REQUEST_READY  = 0x08,
	ATA_STATUS_DRIVE_SEEK_COMPLETE = 0x10,
	ATA_STATUS_DRIVE_WRITE_FAULT   = 0x20,
	ATA_STATUS_DRIVE_READY         = 0x40,
	ATA_STATUS_BUSY                = 0x80
}
ATA_Status;

typedef enum
{
	ATA_NO_ADDRESS_MARK_ERROR      = 0x01,
	ATA_TRACK0_NOT_FOUND_ERROR     = 0x02,
	ATA_COMMAND_ABORTED_ERROR      = 0x04,
	ATA_MEDIA_CHANGE_REQUEST_ERROR = 0x08,
	ATA_ID_MARK_NOT_FOUND_ERROR    = 0x10,
	ATA_MEDIA_CHANGED_ERROR        = 0x20,
	ATA_UNCORRECTABLE_DATA_ERROR   = 0x40,
	ATA_BAD_BLOCK_ERROR            = 0x80
}
ATA_Error;

typedef enum
{
	ATA_READ_PIO_COMMAND        = 0x20,
	ATA_READ_PIO_EXT_COMMAND    = 0x24,
	ATA_READ_DMA_COMMAND        = 0xC8,
	ATA_READ_DMA_EXT_COMMAND    = 0x25,
	ATA_WRITE_PIO_COMMAND       = 0x30,
	ATA_WRITE_PIO_EXT_COMMAND   = 0x34,
	ATA_WRITE_DMA_COMMAND       = 0xCA,
	ATA_WRITE_DMA_EXT_COMMAND   = 0x35,
	ATA_FLUSH_CACHE_COMMAND     = 0xE7,
	ATA_FLUSH_CACHE_EXT_COMMAND = 0xEA,
	ATA_PACKET_COMMAND          = 0xA0,
	ATA_IDENTIFY_PACKET_COMMAND = 0xA1,
	ATAPI_READ_COMMAND          = 0xA8,
	ATAPI_EJECT_COMMAND         = 0x1B,
	ATA_IDENTIFY_COMMAND        = 0xEC
}
ATA_Command;


void configure_intel_IDE(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 'c', 'n', 'c', 0};
	pci->kernel->print(message, 8086, ' ', 7010, '\n');
}