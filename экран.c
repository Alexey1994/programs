#include <Windows/user32.c>
#include <Windows/dxva2.c>

#include <view.c>
#include <memory.c>


/*stdcall Number32 enum_monitors(Byte* current_monitor)
{
	return current_monitor;
}*/

void print_monitors()
{
	Display display;
	Number  i;

	display.structure_size = sizeof(display);

	for(i = 0; ; ++i)
	{
		if(!EnumDisplayDevicesA(0, i, &display, DISPLAY_IS_PRIMARY_DEVICE))
			break;

		print("ssss",
			display.device_name, " ",
			display.device_context_name, "\n");
	}
}


Number get_number_of_displays()
{
	Display display;
	Number  i;
	Number  number_of_displays;

	display.structure_size = sizeof(display);
	number_of_displays = 0;

	for(i = 0; ; ++i)
	{
		if(!EnumDisplayDevicesA(0, i, &display, DISPLAY_IS_PRIMARY_DEVICE))
			break;

		++number_of_displays;
	}

	return number_of_displays;
}


Boolean set_display_settings(Number display_number, Number width, Number height, Number frequency)
{
	Display      display;
	Dispaly_Mode display_mode;

	display.structure_size = sizeof(display);

	if(!EnumDisplayDevicesA(0, display_number, &display, DISPLAY_IS_PRIMARY_DEVICE))
		return 0;

	display_mode.structure_size = sizeof(display_mode);
	EnumDisplaySettingsA(display.device_name, ENUM_CURRENT_SETTINGS, &display_mode);
	
	display_mode.width     = width;
	display_mode.height    = height;
	display_mode.frequency = frequency;
	ChangeDisplaySettingsA(&display_mode, CDS_UPDATEREGISTRY);

	return 1;
}


Boolean is_running = 1;
Number  number_of_displays;

View main_view = {
	.x = 20,
	.y = 20,
	.height = 380,
	.name = "Настройки экранов"
};

Button apply_button = {
	.x = 10,
	.y = 310,
	.width = 100,
	.height = 32,
	.text = "Применить"
};

List_Box* lists_of_display_modes;
Number*   mode_indexes;


void main_view_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	Number       display_index;
	Number       new_mode_index;
	List_Box*    list_of_display_modes;
	Display      display;
	Dispaly_Mode display_mode;

	switch(message)
	{
		case COMMAND_MESSAGE:
			if(parameter2 == apply_button.window)
			{
				//print("s", "button 1 нажата\n");
				display.structure_size = sizeof(display);

				for(display_index = 0; display_index < number_of_displays; ++display_index)
				{
					EnumDisplayDevicesA(0, display_index, &display, DISPLAY_IS_PRIMARY_DEVICE);
					EnumDisplaySettingsA(display.device_name, mode_indexes[display_index], &display_mode);
					ChangeDisplaySettingsA(&display_mode, CDS_UPDATEREGISTRY);
				}
			}
			else
			{
				for(display_index = 0; display_index < number_of_displays; ++display_index)
				{
					list_of_display_modes = lists_of_display_modes + display_index;

					if(parameter2 == list_of_display_modes->window)
					{
						if(parameter1 >> 16 == LBN_SELCHANGE)
						{
							new_mode_index = SendMessageA(list_of_display_modes->window, LB_GETCURSEL, 0, 0);
							//print("nsns", display_index, " ", new_mode_index, " изменено\n");
							mode_indexes[display_index] = new_mode_index;
						}
						
						break;
					}
				}
			}
			break;

		case CREATE_MESSAGE:
		{
			//print("s", "CREATE_MESSAGE\n");
			break;
		}

		case DESTROY_MESSAGE:
			//print("s", "DESTROY_MESSAGE\n");
			is_running = 0;
			break;
	}
}

void draw_UI()
{
	Display      display;
	Dispaly_Mode current_display_mode;
	Dispaly_Mode display_mode;
	Number       display_mode_index;
	Number       field_position;
	Byte         name[256];

	Text*        name_text;
	Text*        current_mode_text;
	List_Box*    list_of_display_modes;
	Number       display_index;

	number_of_displays = get_number_of_displays();
	mode_indexes = allocate_memory(number_of_displays * sizeof(Number));
	lists_of_display_modes = allocate_memory(number_of_displays * sizeof(List_Box));
	main_view.width = number_of_displays * 170 + 20;
	
	initialize_view(&main_view, &main_view_message_handler);
	create_button(&main_view, &apply_button);

	display.structure_size = sizeof(display);
	display_mode.structure_size = sizeof(display_mode);
	current_display_mode.structure_size = sizeof(current_display_mode);

	for(display_index = 0; display_index < number_of_displays; ++display_index)
	{
		EnumDisplayDevicesA(0, display_index, &display, DISPLAY_IS_PRIMARY_DEVICE);

		name_text = allocate_memory(sizeof(Text));
		name_text->x = 10 + display_index * 170;
		name_text->y = 10;
		name_text->width = 160;
		name_text->height = 20;
		name_text->text = display.device_name;
		create_text(&main_view, name_text);

		EnumDisplaySettingsA(display.device_name, ENUM_CURRENT_SETTINGS, &current_display_mode);

		print_in_null_terminated_bytes(name, 256, "nsnsnsns",
				current_display_mode.width, "x", current_display_mode.height, "x",
				current_display_mode.bits_per_pixel, " ",
				current_display_mode.frequency, " Gz");

		current_mode_text = allocate_memory(sizeof(Text));
		current_mode_text->x = 10 + display_index * 170;
		current_mode_text->y = 25;
		current_mode_text->width = 160;
		current_mode_text->height = 100;
		current_mode_text->text = name;
		create_text(&main_view, current_mode_text);

		list_of_display_modes = lists_of_display_modes + display_index;
		list_of_display_modes->x = 10 + display_index * 170;
		list_of_display_modes->y = 57;
		list_of_display_modes->width = 160;
		list_of_display_modes->height = 245;
		list_of_display_modes->text ="Режимы";
		create_list_box(&main_view, list_of_display_modes);
		
		for(display_mode_index = 0; ; ++display_mode_index)
		{
			if(!EnumDisplaySettingsA(display.device_name, display_mode_index, &display_mode))
				break;

			if(
				current_display_mode.width == display_mode.width &&
				current_display_mode.height == display_mode.height &&
				current_display_mode.bits_per_pixel == display_mode.bits_per_pixel &&
				current_display_mode.frequency == display_mode.frequency &&
				current_display_mode.display.position.x == display_mode.display.position.x &&
				current_display_mode.display.position.y == display_mode.display.position.y &&
				current_display_mode.display.orientation == display_mode.display.orientation &&
				current_display_mode.color == display_mode.color
			)
			{
				mode_indexes[display_index] = display_mode_index;
				print("ns", mode_indexes[display_index], "\n");
			}

			print_in_null_terminated_bytes(name, 256, "nsnsnsns",
				display_mode.width, "x", display_mode.height, "x",
				display_mode.bits_per_pixel, " ",
				display_mode.frequency, " Gz");

			field_position = SendMessageA(list_of_display_modes->window, LB_ADDSTRING, 0, name);
			//SendMessageA(list_of_display_modes->window, LB_SETITEMDATA, field_position, 0);
		}

		SendMessageA(list_of_display_modes->window, LB_SETCURSEL, mode_indexes[display_index], 0);
	}

	draw_views(&is_running);
}


void mmain()
{
	draw_UI();

	//print_monitors();
	//set_display_settings(0, 1024, 768, 100);


	//Display      display;
	//Dispaly_Mode mode;

	//display.structure_size = sizeof(display);
	//print(_Number, EnumDisplayDevicesA(0, 0, &display, DISPLAY_IS_PRIMARY_DEVICE), "\n");

	//mode.structure_size = sizeof(mode);
	//print(_Number, EnumDisplaySettingsA(&display, DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY, &mode), "\n");
	//print("ns", EnumDisplaySettingsA(0, ENUM_CURRENT_SETTINGS, &mode), "\n");
	//EnumDisplaySettingsA(0, DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY, &mode);
	
	/*mode.width = 1024;//1920;
	mode.height = 768;//1080;
	ChangeDisplaySettingsA(&mode, CDS_UPDATEREGISTRY);*/

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


stdcall Number32 WinMain(Byte* instance, Byte* previouse_instance, Byte* arguments, Number32 show_options)
{
	draw_UI();
}