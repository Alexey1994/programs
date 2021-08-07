#ifndef KERNEL_INCLUDED
#define KERNEL_INCLUDED


#include <types.c>


typedef void (*Module)(struct Kernel* kernel, Number module_address);


typedef struct
{
	Number memory_size;

	Number8  (*in_8)   (Number16 port);
	void     (*out_8)  (Number16 port, Number8 data);
	Number16 (*in_16)  (Number16 port);
	void     (*out_16) (Number16 port, Number16 data);
	Number32 (*in_32)  (Number16 port);
	void     (*out_32) (Number16 port, Number32 data);

	void (*add_interrupt_handler) (Number8 interrupt_number, void(*handler)(Byte* data), Byte* data);

	void (*print)(Byte* chunk, ...);

	Module (*load_module) (Number16* directory, Number16* path);
}
Kernel;


#endif//KERNEL_INCLUDED