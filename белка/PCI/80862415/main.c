#include <types.c>
#undef print
#include <interfaces/PCI.h>


typedef enum
{
	AC97_INTERRUPT_ON_COMPLETION = 0x8000,
	AC97_BUFFER_UNDERRUN_POLICY  = 0x4000
}
AC97_Buffer_Type;


typedef struct
{
	Byte*            data;
	Number16         length;//0xFFFE max
	AC97_Buffer_Type type; 
}
AC97_Buffer_Descriptor;


//https://github.com/mirror/vbox/blob/master/src/VBox/Devices/Audio/DevIchAc97.cpp
//http://websvn.kolibrios.org/filedetails.php?repname=Kolibri+OS&path=%2Fdrivers%2Faudio%2Fintelac97.asm
typedef enum
{
	AC97_RESET             = 0x0000,
	AC97_MASTER_VOLUME     = 0x0002,
	AC97_AUX_OUT_VOLUME    = 0x0004,
	AC97_MONO_VOLUME       = 0x0006,
	AC97_MASTER_TONE       = 0x0008,
	AC97_PC_BEEP_VOLUME    = 0x000A,
	AC97_PHONE_VOLUME      = 0x000C,
	AC97_MIC_VOLUME        = 0x000E,
	AC97_LINE_IN_VOLUME    = 0x0010,
	AC97_CD_VOLUME         = 0x0012,
	AC97_VIDEO_VOLUME      = 0x0014,
	AC97_AUX_IN_VOLUME     = 0x0016,
	AC97_PCM_OUT_VOLUME    = 0x0018,
	AC97_REC_SELECT        = 0x001A,
	AC97_REC_GAIN          = 0x001C,
	AC97_REC_GAIN_MIC      = 0x001E,
	AC97_REC_GEN           = 0x0020,
	AC97_3D_CONTROL        = 0x0022,
	AC97_PAGE              = 0x0024,
	AC97_POWERDOWN         = 0x0026,
	AC97_EXT_AUDIO         = 0x0028,
	AC97_EXT_ST            = 0x002A,
	AC97_FRONT_RATE        = 0x002C,
	AC97_PCM_SURR_RATE     = 0x002E,
	AC97_LFE_RATE          = 0x0030,
	AC97_PCM_IN_RATE       = 0x0032,
	AC97_CENT_LFE_VOLUME   = 0x0036,
	AC97_SURR_VOLUME       = 0x0038,
	AC97_SPDIF_CONTROL     = 0x003A,
	AC97_PAGE0             = 0x0060,
	AC97_PAGE1             = 0x0062,
	AC97_PAGE2             = 0x0064,
	AC97_PAGE3             = 0x0066,
	AC97_PAGE4             = 0x0068,
	AC97_PAGE5             = 0x006A,
	AC97_PAGE6             = 0x006C,
	AC97_PAGE7             = 0x006E,
	AC97_VENDOR_ID1        = 0x007C,
	AC97_VENDOR_ID2        = 0x007E
}
AC97_Commands;


//https://github.com/Zero3K/AC97/blob/master/driver/ichreg.h
typedef enum
{
	AC97_PCM_IN_BUFFER_ADDRESS              = 0x00,
	AC97_PCM_IN_INDEX                       = 0x04,
	AC97_PCM_IN_LAST_INDEX                  = 0x05,
	AC97_PCM_IN_STATUS                      = 0x06,
	AC97_PCM_IN_POSITiON_IN_CURRENT_BUFFER  = 0x08,
	AC97_PCM_IN_PREFETCH_INDEX              = 0x0A,
	AC97_PCM_IN_CONTROL                     = 0x0B,
	
	AC97_PCM_OUT_BUFFER_ADDRESS             = 0x10,
	AC97_PCM_OUT_INDEX                      = 0x14,
	AC97_PCM_OUT_LAST_INDEX                 = 0x15,
	AC97_PCM_OUT_STATUS                     = 0x16,
	AC97_PCM_OUT_POSITION_IN_CURRENT_BUFFER = 0x18,
	AC97_PCM_OUT_PREFETCH_INDEX             = 0x1A,
	AC97_PCM_OUT_CONTROL                    = 0x1B,
	
	AC97_MIC_IN_BUFFER_ADDRESS              = 0x20,
	AC97_MIC_IN_INDEX                       = 0x24,
	AC97_MIC_IN_LAST_INDEX                  = 0x25,
	AC97_MIC_IN_STATUS                      = 0x26,
	AC97_MIC_IN_POSITION_IN_CURRENT_BUFFER  = 0x28,
	AC97_MIC_IN_PREFETCH_INDEX              = 0x2A,
	AC97_MIC_IN_CONTROL                     = 0x2B,

	AC97_GLOBAL_CONTROL                     = 0x2C,
	AC97_GLOBAL_STATUS                      = 0x30,
	AC97_CODEC_ACCESS_SEMIPHORE             = 0x34
}
AC97_Controller_Commands;


void interrupt_handler(PCI* pci);


void configure_AC97(PCI* pci, PCI_Device* device, Number module_address)
{
	Byte message[] = {'c', 'c', 'c', 'c', 'c', 0};
	pci->kernel->print(message, 'A', 'C', '9', '7', '\n');


	//Number8 interrupt_line = pci->read_from_PCI_device_config(pci->kernel, device, PCI_INTERRUPT_LINE);
	pci->kernel->add_interrupt_handler(32 + device->config.interrupt_line, (Number32)&interrupt_handler + module_address, pci);

	//init
	Number32 i;

	pci->write_in_PCI_device_config(pci->kernel, device, PCI_COMMAND, pci->read_from_PCI_device_config(pci->kernel, device, PCI_COMMAND) | 5); //enable commands

	Number32 codec;
	Number32 controller;

	codec = pci->get_PCI_IO_address(device, 0);
	controller = pci->get_PCI_IO_address(device, 1);

	Number32 volume;
	volume = 0; //0 - max volume, 63 - low
	
	pci->kernel->out_16(codec + AC97_RESET, 42);
	pci->kernel->out_8(codec + AC97_PAGE0, 2);

	//delay 100ms
	for(i = 0; i < 1000000; ++i){}

	pci->kernel->out_16(codec + AC97_MASTER_VOLUME, volume);
	pci->kernel->out_16(codec + AC97_MONO_VOLUME, volume);
	pci->kernel->out_16(codec + AC97_PC_BEEP_VOLUME, volume);
	pci->kernel->out_16(codec + AC97_PCM_OUT_VOLUME, volume);

	//delay 10ms
	for(i = 0; i < 1000000; ++i){}

	if(!pci->kernel->in_16(codec + AC97_EXT_AUDIO) & 1)
	{

	}
	else
	{
		pci->kernel->out_16(codec + AC97_EXT_AUDIO, pci->kernel->in_16(codec + AC97_EXT_AUDIO) | 1);

		//delay 10ms
		for(i = 0; i < 1000000; ++i){}

		pci->kernel->out_16(codec + AC97_FRONT_RATE, 44100);
		pci->kernel->out_16(codec + AC97_PCM_IN_RATE, 44100);

		//delay 10ms
		for(i = 0; i < 1000000; ++i){}
	}
	//

	AC97_Buffer_Descriptor* buffer;

	buffer = 1024 * 1024 + 65536;

	buffer[0].data   = 1024 * 1024 + 65536 * 2;
	buffer[0].length = 65535;
	buffer[0].type   = AC97_INTERRUPT_ON_COMPLETION;

	buffer[1].data   = buffer[0].data + buffer[0].length + 1;
	buffer[1].length = 65535;
	buffer[1].type   = AC97_BUFFER_UNDERRUN_POLICY;

	for(i = 0; i <= buffer[0].length; ++i)
		buffer[0].data[i] = 100 + i % 50;

	for(i = 0; i <= buffer[1].length; ++i)
		buffer[1].data[i] = 200 + i % 50;

	//play
	pci->kernel->out_32(controller + AC97_PCM_OUT_BUFFER_ADDRESS, buffer);
	pci->kernel->out_8(controller + AC97_PCM_OUT_LAST_INDEX, 2);
	pci->kernel->out_8(controller + AC97_PCM_OUT_CONTROL, 0x15);
	//
}


void interrupt_handler(PCI* pci)
{
	//print("next buffer\n", 0);
	Byte message[] = {'n', 'n', 'c', 0};
	pci->kernel->print(message, 8086, 2415, '\n');

	Number16* t = 0xB8000;

	t[0] = 1 + (3 << 8);
}