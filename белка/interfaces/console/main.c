#include <interfaces/video.h>
#undef print


void initialize_video_subsystem(Kernel* kernel, Number module_address)
{
	kernel->print(module_address + "s", module_address + "hi from console\n");
}