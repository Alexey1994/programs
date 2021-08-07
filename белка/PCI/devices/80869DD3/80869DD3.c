#include <types.c>
#undef print
#include "../../PCI.h"


void configure_AHCI_disk(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 0};
	pci->kernel->print(message, 8086);
	//pci->kernel->print(module_address + "n", 8086);
}