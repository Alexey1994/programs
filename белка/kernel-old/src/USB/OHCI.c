#ifndef USB_OHCI_INCLUDED
#define USB_OHCI_INCLUDED


#include "../PCI.c"


typedef enum
{
	OHCI_HOST_CONTROLLER_RESET = 1,
	OHCI_CONTROL_LIST_FILLED = 2,
	OHCI_BULK_LIST_FILLED = 4
}
OHCI_Command_Status;


typedef enum
{
	OHCI_INTERRUPT_MASTER_ENABLE = 0x80000000
}
OHCI_Interrupt;


typedef struct
{
	Number32 flags; //bits 0-6: function address
	                //bits 7-10: endpoint number
	                //bits 11-12: Direction (1 - out, 2 - in), если 0 или 3 то определено в PID transfer descriptor
	                //bit 13: speed bit, 0 - full speed, 1 - low speed
	                //bit 14: skip bit
	                //bit 15: format bit, 0 for control, bulk, interrupt, 1 for isochronous endpoints
	                //bits 16-26: maximum packet size
	Number32 tail;
	Number32 head;
	Number32 next_ED;
}
OHCI_Endpoint;


typedef struct
{
	Number32 flags; //bits 0-6: function address
	                //bits 7-10: endpoint number
	                //bits 11-12: Direction (1 - out, 2 - in), если 0 или 3 то определено в PID transfer descriptor
	                //bit 13: speed bit, 0 - full speed, 1 - low speed
	                //bit 14: skip bit
	                //bit 15: format bit, 0 for control, bulk, interrupt, 1 for isochronous endpoints
	                //bits 16-26: maximum packet size
	Number32 tail;
	Number32 head;
	Number32 next_ED;
}
OHCI_Control;


typedef struct
{
	//hardware
	Number32 interrupts[32];
	Number16 frame_number;
	Number16 pad1;
	Number32 done_head;
	Byte     reserved[120];
/*
	//software
	OHCI_Endpoint endpoints[63];
	OHCI_Endpoint control_endpoint;
	OHCI_Endpoint bulk_endpoint;
	Number32      mmio_base;
	Number32      powered_up;
	Number32      done_list;
	Number32      done_list_end_address;
	Number32      EHCI_companion;*/
}
OHCI_HCCA; //host controller communication area

/*
typedef enum
{
	OHCI_HOST_RESET = 0,
	OHCI_HOST_RESUME = 1,
	OHCI_HOST_OPERATIONAL = 2,
	OHCI_HOST_SUSPEND = 3
}
OHCI_Command_Status;*/


#define OHCI_FRAME_INTERVAL 0x2EDF
#define OHCI_DEFAULT_FRAME_INTERVAL ((((6 * (OHCI_FRAME_INTERVAL - 210)) / 7) << 16) | OHCI_FRAME_INTERVAL)

#define OHCI_PERIODIC_START 0x00002A27


typedef struct
{
	//control and status
	Number32 revision;
	Number32 control;
	Number32 command_status;
	Number32 interrupt_status;
	Number32 interrupt_enable;
	Number32 interrupt_disable;

	//memory pointer
	Number32 HCCA;
	Number32 PeriodCurrentED;
	Number32 ControlHeadED;
	Number32 ControlCurrentED;
	Number32 BulkHeadED;
	Number32 BulkCurrentED;
	Number32 DoneHead;

	//frame counter
	Number32 FmInterval;
	Number32 FmRemaining;
	Number32 FmNumber;
	Number32 PeriodicStart;
	Number32 LSThreshold;

	//root hub
	Number32 RhDescriptorA;
	Number32 RhDescriptorB;
	Number32 RhStatus;
	Number32 RhPortStatus;
}
OHCI_Interface;


void OHCI_interrupt()
{
	print("GHTHSDFYBT ", 0);//прерывание
}


OHCI_Endpoint endpoints[128];


void configure_USB_OHCI_device(PCI_Device* device)
{
	Number i;

	if(device->config.class == 12 && device->config.subclass == 3 && device->config.prog_IF == 16)
	{
		//print("EGI YFQLTY\n", 0);//УПШ НАЙДЕН

		OHCI_Interface* interface;

		interface = device->config.BARs[0] & 0b11111111111111111111111111110000;

		//interface->control = 0;

		//OHCI_Pipe p;
		//interface->ControlHeadED = &p;

		print(_Number, interface->RhPortStatus, "\n", 0);

		/*
		interface->interrupt_enable = OHCI_INTERRUPT_MASTER_ENABLE;

		interface->FmInterval = OHCI_DEFAULT_FRAME_INTERVAL;
		interface->PeriodicStart = OHCI_PERIODIC_START;

		endpoints[0].next_ED = 0;
		interface->ControlHeadED = endpoints;

		OHCI_HCCA* hcca;
		hcca = interface->HCCA;

		Number i;
		for(i = 0; i < 32; ++i)
			hcca->interrupts[i] = &OHCI_interrupt;
		*/

/*
		if(interface->interrupt_enable && interface->interrupt_status)
		{
			print("set interrupt\n", 0);
			interface->interrupt_disable = OHCI_INTERRUPT_MASTER_ENABLE;

			interface->interrupt_enable = OHCI_INTERRUPT_MASTER_ENABLE;
		}

		print(
			&write_Number, interface->interrupt_enable,
			"\n", 0
		);*/

		//Number i;
		//for(i = 0; i < 32; ++i)
		//{
		//	print(
		//		&write_Number, /*(hcca->endpoints[i].flags >> 6) & 0b1111,*/hcca->interrupts[i],
		//		" ", 0
		//	);
		//}
	}
}


void configure_USB_OHCI_devices()
{
	find_PCI_devices(&configure_USB_OHCI_device);
}


#endif//USB_OHCI_INCLUDED