#include <kernel.h>


Boolean set_mode(Number width, Number height, Number bits_per_pixel, Number frequency);


void start_module(Kernel* kernel, Number module_address)
{
	Display_Context* display;
	Module           vga_module;

	display = kernel->display = kernel->allocate_global_memory(sizeof(Display_Context));

	display->set_mode = module_address + (Byte*)&set_mode;

	vga_module = kernel->load_module(module_address + "DRIVERS/DISPLAY", module_address + "VGA");
	vga_module(kernel, vga_module);
}


Boolean set_mode(Number width, Number height, Number bits_per_pixel, Number frequency)
{

}