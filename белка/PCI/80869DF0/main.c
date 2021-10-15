#include <types.c>
#undef print
#include <interfaces/PCI.h>


void configure_realtek_wifi(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'n', 0};
	pci->kernel->print(message, 8086);
	//pci->kernel->print(module_address + "n", 8086);
}