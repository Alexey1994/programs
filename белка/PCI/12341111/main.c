#include <types.c>
#undef print
#include <interfaces/PCI.h>


typedef enum
{
	VBE_DISPI_INDEX_ID          = 0,
	VBE_DISPI_INDEX_XRES        = 1,
	VBE_DISPI_INDEX_YRES        = 2,
	VBE_DISPI_INDEX_BPP         = 3,
	VBE_DISPI_INDEX_ENABLE      = 4,
	VBE_DISPI_INDEX_BANK        = 5,
	VBE_DISPI_INDEX_VIRT_WIDTH  = 6,
	VBE_DISPI_INDEX_VIRT_HEIGHT = 7,
	VBE_DISPI_INDEX_X_OFFSET    = 8,
	VBE_DISPI_INDEX_Y_OFFSET    = 9
}
VBE_Dispi;

typedef enum
{
	VBE_DISPI_DISABLED    = 0,
	VBE_DISPI_ENABLED     = 1,
	VBE_DISPI_LFB_ENABLED = 0x40,
	VBE_DISPI_NOCLEARMEM  = 0x80
}
VBE_Dispi2;


void write_in_BGA(Kernel* kernel, Number16 index, Number16 data);


void configure_Qemu_video(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 0};
	pci->kernel->print(message, 12341111);

	pci->print_PCI_device(pci->kernel, device);

	write_in_BGA(pci->kernel, VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
	write_in_BGA(pci->kernel, VBE_DISPI_INDEX_XRES, 640);
	write_in_BGA(pci->kernel, VBE_DISPI_INDEX_YRES, 480);
	write_in_BGA(pci->kernel, VBE_DISPI_INDEX_BPP, 32);
	write_in_BGA(pci->kernel, VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED);


	Number32* framebuffer = pci->get_PCI_MMIO_address(device, 0);

	Number i;

	for(i = 0; i < 12000; ++i)
		framebuffer[i] = 255;
}


#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF


void write_in_BGA(Kernel* kernel, Number16 index, Number16 data)
{
	kernel->out_16(VBE_DISPI_IOPORT_INDEX, index);
	kernel->out_16(VBE_DISPI_IOPORT_DATA, data);
}