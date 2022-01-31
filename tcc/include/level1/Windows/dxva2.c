#ifndef SYSTEM_DXVA2_INCLUDED
#define SYSTEM_DXVA2_INCLUDED


#include <types.c>


typedef struct
{
	Byte*    handle;
	Number16 name[128];
}
Physical_Monitor;


import Number32 GetMonitorBrightness(Physical_Monitor* monitor, Number32* minimum_brightness, Number32* current_brightness, Number32* maximum_brightness);
import Number32 SetMonitorBrightness(Physical_Monitor* monitor, Number32 new_brightness);

import Number32 GetPhysicalMonitorsFromHMONITOR(Physical_Monitor* monitor, Number32 number_of_physical_monitors, Physical_Monitor* physical_monitors);
import Number32 GetNumberOfPhysicalMonitorsFromHMONITOR(Physical_Monitor* monitor, Number32* number_of_monitors);


#endif//SYSTEM_DXVA2_INCLUDED