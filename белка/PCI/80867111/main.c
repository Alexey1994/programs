#include <types.c>
#undef print
#include <interfaces/PCI.h>


void configure_intel_IDE(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 'n', 0};
	//pci->kernel->print(message, 8086, 7111);
	//pci->kernel->print(module_address + "n", 8086);
}
