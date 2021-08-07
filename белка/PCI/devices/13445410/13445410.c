#include <types.c>
#undef print
#include "../../PCI.h"


void configure_NVM(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 0};
	pci->kernel->print(message, 1344);
	//pci->kernel->print(module_address + "n", 1344);
}