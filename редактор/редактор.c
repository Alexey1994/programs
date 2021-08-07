#include <system/file.c>
#include <memory.c>
#include <data-structures/stack.c>
#include <view.c>
#include <string.c>


typedef struct
{
	stdcall Byte* (*list_load)(Byte* parent_window, Byte* file_to_load, Number32 show_flags);
}
Plugin;


Stack plugins = {0};


void on_find_plugin(Stack* path_to_plugin, Bit16* name, Boolean is_directory, Number size)
{
	Bit16  bytes[6];
	Number i;

	remove_bytes_from_stack(path_to_plugin, bytes, sizeof(bytes));

	for(i = 0; name[i]; ++i)
		add_bytes_in_stack(path_to_plugin, name + i, sizeof(*name));

	add_bytes_in_stack(path_to_plugin, L"", 2);
}

void on_find_plugin_folder(Byte* context, Bit16* name, Boolean is_directory, Number size)
{
	if(is_directory && (!compare_null_terminated_words(name, L".") || !compare_null_terminated_words(name, L"..")))
		return;

	if(is_directory)
	{
		Number i;
		Stack  path_to_plugin;
		initialize_stack(&path_to_plugin, 128);
		add_bytes_in_stack(&path_to_plugin, L"plugins/wlx/", 24);
		for(i = 0; name[i]; ++i)
			add_bytes_in_stack(&path_to_plugin, name + i, sizeof(*name));
		add_bytes_in_stack(&path_to_plugin, L"/*.wlx", 14);
		find_files(path_to_plugin.data, &on_find_plugin, &path_to_plugin);

		Byte* plugin_dll = LoadLibraryW(path_to_plugin.data);
		if(plugin_dll)
		{
			Plugin plugin;

			plugin.list_load =  GetProcAddress(plugin_dll, "ListLoad");

			if(plugin.list_load)
			{
				add_bytes_in_stack(&plugins, &plugin, sizeof(plugin));
				print(_null_terminated_words, name);
				printf(" loaded\n");
			}
		}

		deinitialize_stack(&path_to_plugin);
	}
}

void find_plugins()
{
	initialize_stack(&plugins, 128);
	find_files(L"plugins/wlx/*", &on_find_plugin_folder, 0);
}

/*
void find_files()
{
	Find_File_Information file_information;
	Byte* file_finder;

	file_finder = FindFirstFileW(L".\\plugins\\*", &file_information);
	if(file_finder)
	{
		print(file_information.file_name);
		printf("\n");

		while(FindNextFileW(file_finder, &file_information))
		{
			print(file_information.file_name);
			printf("\n");
		}
		FindClose(file_finder);
	}
}*/


View view_1 = {
	.x = 0,
	.y = 0,
	.width = 800,
	.height = 600,
	.name = L"View1"
};
/*
Scrollbar view_1 = {
	.x = 0,
	.y = 0,
	.width = 800,
	.height = 600,
};*/

View view_2 = {
	.x = 0,
	.y = 0,
	.width = 1024,
	.height = 600,
	.name = L"View2"
};


typedef struct
{
	Number16 path[256];
	Number16 name[256];
	Boolean  is_directory;

	Button button;
}
Drive;


Stack    current_drives;
Number16 current_path[256] = {0};


void set_drive_text(Drive* drive, Number16* text)
{
	Number16* path = drive->path;
	Number16* name = drive->name;

	drive->button.text = text;

	while(*path)
	{
		*text = *path;
		++text;
		++path;
	}
	*text = '\0';
}


void create_drive(Drive* drive)
{
	//print(drive->path, " ", drive->name, "\n");

	drive->button.x = 0;
	drive->button.y = current_drives.size / sizeof(Drive) * 25;
	drive->button.width = 200;
	drive->button.height = 25;

	set_drive_text(drive, allocate_memory(256));

	create_button(&view_1, &drive->button);
}


void destroy_drive(Drive* drive)
{
	DestroyWindow(drive->button.window);
}


void draw_logical_drives()
{
	Number32 drives;
	Drive*   drive;
	Number   i;

	drives = GetLogicalDrives();

	for(i = 0; i < sizeof(Number32) * 8; ++i)
	{
		if((drives >> i) & 1)
		{
			drive = allocate_bytes_in_stack(&current_drives, sizeof(Drive));
			drive->path[0] = i + 'A';
			drive->path[1] = ':';
			drive->path[2] = '\0';
			//snprintf(drive->path, 4, "%c:\\", i + 'A');
			GetVolumeInformationW(drive->path, drive->name, 256, 0, 0, 0, 0, 0);
			drive->is_directory = 1;

/*
			Drive_Type type = GetDriveTypeA(buf);
			switch(type)
			{
				case DRIVE_UNKNOWN:
					printf("unknown");
					break;

				case DRIVE_NO_ROOT_DIR:
					printf("no root dir");
					break;

				case DRIVE_REMOVABLE:
					printf("removable");
					break;

				case DRIVE_FIXED:
					printf("fixed");
					break;

				case DRIVE_REMOTE:
					printf("remote");
					break;

				case DRIVE_CDROM:
					printf("CD-ROM");
					break;

				case DRIVE_RAMDISK:
					printf("RAM disc");
					break;
			}*/

			create_drive(drive);
		}
	}
}


void view_2_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{

}


void draw_path(Drive* new_drive)
{
	Find_File_Information file_information;
	Byte*                 file_finder;
	Drive*                drive;
	Number16*             path;
	Number16              find_path[256];

	if(new_drive->is_directory)
	{
		Number16* new_path = current_path;
		Number16* current_find_path = find_path;

		while(*new_path)
		{
			*current_find_path = *new_path;
			++current_find_path;
			++new_path;
		}

		path = new_drive->path;
		while(*path)
		{
			*new_path = *path;
			*current_find_path = *path;
			++current_find_path;
			++new_path;
			++path;
		}
		*new_path = '\\';
		++new_path;
		*new_path = '\0';
		*current_find_path = '\\';
		++current_find_path; 
		*current_find_path = '*';
		++current_find_path;
		*current_find_path = '\0';


		Number i;

		for(i = 0; i < current_drives.size / sizeof(Drive); ++i)
			destroy_drive((Drive*)current_drives.data + i);

		clean_stack(&current_drives);

		//Windows_Rectangle rectangle = {0, 0, 800, 600};
		//FillRect(GetDC(view_1.window), &rectangle, CreateSolidBrush(255 + (255 << 8) + (255 << 16)));


		file_finder = FindFirstFileW(find_path, &file_information);
		if(file_finder)
		{
			drive = allocate_bytes_in_stack(&current_drives, sizeof(Drive));
			Number16* drive_path_in = file_information.file_name;
			Number16* drive_path = drive->path;

			while(*drive_path_in)
			{
				*drive_path = *drive_path_in;
				++drive_path;
				++drive_path_in;
			}
			*drive_path = '\0';

			drive->name[0] = '\0';
			drive->is_directory = file_information.attributes & DIRECTORY_FILE_ATTRIBUTE;
			create_drive(drive);

			while(FindNextFileW(file_finder, &file_information))
			{
				drive = allocate_bytes_in_stack(&current_drives, sizeof(Drive));
				Number16* drive_path_in = file_information.file_name;
				Number16* drive_path = drive->path;

				while(*drive_path_in)
				{
					*drive_path = *drive_path_in;
					++drive_path;
					++drive_path_in;
				}
				*drive_path = '\0';

				drive->name[0] = '\0';
				drive->is_directory = file_information.attributes & DIRECTORY_FILE_ATTRIBUTE;
				create_drive(drive);
			}
			FindClose(file_finder);
		}
	}
	else
	{
		Number16* new_path = current_path;
		Number16* current_find_path = find_path;

		while(*new_path)
		{
			*current_find_path = *new_path;
			++current_find_path;
			++new_path;
		}

		path = new_drive->path;
		while(*path)
		{
			*current_find_path = *path;
			++current_find_path;
			++path;
		}
		*current_find_path = '\0';

		Byte utf8_path[256];
		WideCharToMultiByte(CP_ACP, 0, find_path, 256, utf8_path, 256, 0, 0);

		if(view_2.window)
			DestroyWindow(view_2.window);

		initialize_view(&view_2, &view_2_message_handler);

		Number i;
		Plugin* plugin = plugins.data;
		for(i = 0; i < plugins.size / sizeof(Plugin); ++i)
		{
			if(plugin->list_load(view_2.window, utf8_path, 0))
				break;

			++plugin;
		}
	}
}


void view_1_message_handler(View* view, Number message, Number parameter1, Number parameter2)
{
	Number i;

	switch(message)
	{
		case COMMAND_MESSAGE:
			/*if(parameter2 == button_1_1.window)
			{
				printf("button 1 click\n");
			}*/
			for(i = 0; i < current_drives.size / sizeof(Drive); ++i)
			{
				if(((Drive*)current_drives.data + i)->button.window == parameter2)
				{
					draw_path((Drive*)current_drives.data + i);
				}
			}
			break;
	}
}

static stdcall Number32 update_window_state2(Byte* window, Number32 message, Number32* parameter1, Integer_Number32* parameter2)
{
	return DefWindowProcW(window, message, parameter1, parameter2);
}


stdcall Number32 WinMain(Byte* instance, Byte* previouse_instance, Byte** arguments, Number32 show_options)
{
	initialize_stack(&current_drives, 1024);

	initialize_main_view(&view_2, &view_2_message_handler);
	//create_scrollbar(&view_2, &view_1);
	initialize_subview(&view_1, &view_1_message_handler, &view_2);
	//initialize_view(&view_1, &view_1_message_handler);

	//print_logical_drives();
	find_plugins();

	//Plugin* plugin = plugins.data;
	//print(_Number, plugin->list_load(view_1.window, "d:\\a.mp3", 0));

/*
	Byte* plugin = LoadLibraryW(L"plugins/wlx/MMedia/MMedia.wlx");//LoadLibraryW(L"plugins/wlx/FileInfo.wlx");
	if(plugin)
	{
		stdcall Byte* (*list_load)(Byte* parent_window, Byte* file_to_load, Number32 show_flags);
		
		list_load = GetProcAddress(plugin, "ListLoad");

		if(list_load)
		{
			Byte* result;
			result = list_load(view_1.window, "c:\\a.mp3", 0);
			printf("%d\n", result);
		}
	}*/

	draw_logical_drives();


	for(;;)
	{
		draw_views();
		Sleep(10);
	}
}