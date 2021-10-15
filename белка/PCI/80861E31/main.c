#define M(s) (module_address + (s))


#include <types.c>
#undef print
#include <interfaces/PCI.h>


typedef struct
{
	//capability
	Number8  capability_size;
	Byte     reserved;
	Number16 version;
	Number32 structural_params1;
	Number32 structural_params2;
	Number32 structural_params3;
	Number32 capability_params;
	Number32 doorbell_offset;
	Number32 runtime_registers_space_offset;

	//operational
	Number32 command;
	Number32 status;
	Number32 page_size;
	Number32 reserved[2];
	Number32 device_notification_control;
	Number32 command_ring_control;

}
XHCI;


void configure_USB_3_0(PCI* pci, PCI_Device* device, Number module_address)
{
	//Byte message[] = {'n', 0};
	//pci->kernel->print(message, 8086);
	//pci->kernel->print(module_address + "n", 8086);

	Number32 interface1;
	Number32 interface2;

	interface1 = pci->get_PCI_MMIO_address(device, 0);
	interface2 = pci->get_PCI_MMIO_address(device, 1);

	XHCI* interface = interface1;

	pci->kernel->print(M("ns"), interface->structural_params1, M(" "));
}
