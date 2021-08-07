#ifndef USB_EHCI_INCLUDED
#define USB_EHCI_INCLUDED


typedef struct
{
	Number8  size;
	Number8  reserved;
	Number16 version;
	Number32 structural_parameters;
	Number32 capability_parameters;
	Number32 companion_port_route;
}
EHCI_Capability;


typedef struct
{
	Number32 command;
	Number32 status;
	Number32 interrupt;
	Number32 frame_index;
	Number32 segment_selector;
	Number32 frame_list_address;
	Number32 asynchronus_list_address;
	Number32 pad[10];
	Number32 config;
	Number32 port_status_and_control[32];
}
EHCI_Operation;


void configure_USB_EHCI_device(PCI_Device* device)
{
	EHCI_Capability* capability;
	EHCI_Operation* operation;

	capability = get_PCI_MMIO_address(device, 0);
	operation = (Byte*)capability + capability->size;

	print(_Number, operation->command, "\n");
}


#endif//USB_EHCI_INCLUDED