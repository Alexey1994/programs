#include <system/Windows/user32.c>
#include <system/Windows/dxva2.c>


Number32 enum_monitors(Byte* current_monitor)
{
	return current_monitor;
}


void main()
{
	Display      display;
	Dispaly_Mode mode;

	//display.structure_size = sizeof(display);
	//print(_Number, EnumDisplayDevicesA(0, 0, &display, DISPLAY_IS_PRIMARY_DEVICE), "\n");

	mode.structure_size = sizeof(mode);
	//print(_Number, EnumDisplaySettingsA(&display, DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY, &mode), "\n");
	print(_Number, EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &mode), "\n");
	//EnumDisplaySettingsA(0, DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY, &mode);
	mode.width = 1920;
	mode.height = 1080;
	ChangeDisplaySettingsA(&mode, CDS_UPDATEREGISTRY);

/*
	Byte*            monitor;
	Number32         number_of_physical_monitors;
	Physical_Monitor physical_monitors[16];

	Number32 minimum;
	Number32 current;
	Number32 maximum;
	print(_Number, GetLastError(), "\n");
	monitor = EnumDisplayMonitors(0, 0, &enum_monitors, 0);
	print(_Number, monitor, " <= EnumDisplayMonitors ", _Number, GetLastError(), "\n");
	number_of_physical_monitors = GetNumberOfPhysicalMonitorsFromHMONITOR(monitor, &number_of_physical_monitors);
	print(_Number, GetPhysicalMonitorsFromHMONITOR(monitor, number_of_physical_monitors, physical_monitors), " <= GetPhysicalMonitorsFromHMONITOR ", _Number, GetLastError(), "\n");
	print(_Number, physical_monitors[0].handle, ": ", _null_terminated_words, physical_monitors[0].name, "\n");

	//print(_Number, GetMonitorBrightness(0, &minimum, &current, &maximum), " <= GetMonitorBrightness\n");

	//print(_Number, GetMonitorBrightness\n");
	print(
		_Number, SetMonitorBrightness(physical_monitors->handle, 0),
		" <= SetMonitorBrightness ",
		_Number, GetLastError(),
		"\n"
	);

	//print(_Number, current, "\n");


	print(
		_Number, GetMonitorBrightness(physical_monitors->handle, &minimum, &current, &maximum),
		" <= GetMonitorBrightness ",
		_Number, GetLastError(),
		"\n"
	);
*/
}