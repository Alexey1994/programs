#ifndef SYSTEM_FILE_INCLUDED
#define SYSTEM_FILE_INCLUDED


#include <types.c>


typedef Byte* File;

static Boolean  create_directory(Byte* path);
static Boolean  delete_directory(Byte* path);

static void     find_files (
	Byte* search_path,
	void(*on_find)(
		Byte*    context,
		Byte*    name,
		Boolean  is_directory,
		Number32 size_low,
		Number32 size_hight
	),
	Byte* context
);

static File     create_file                  (Byte* path);
static File     open_file                    (Byte* path);
static Number32 read_bytes_from_file         (File file, Byte* bytes, Number32 size_of_bytes);
static Number32 write_bytes_in_file          (File file, Byte* bytes, Number32 size_of_bytes);
static Number64 get_file_size                (File file);
static void     offset_current_file_position (File file, Number32 position);
static void     set_file_position            (File file, Number32 position);
static Number   get_file_position            (File file);
static void     set_file_end_position        (File file);
static void     close_file                   (File file);
static Boolean  delete_file                  (Byte* path);


#ifdef __WIN32__

#include <Windows/kernel32.c>
#include <string.c>


static Boolean create_directory(Byte* path)
{
	Number32  status;
	Number16* unicode_path;

	unicode_path = convert_utf8_to_unicode(path);
	status = CreateDirectoryW(unicode_path, 0);
	free_memory(unicode_path);

	return status;
}


static Boolean delete_directory(Byte* path)
{
	Number32  status;
	Number16* unicode_path;

	unicode_path = convert_utf8_to_unicode(path);
	status = RemoveDirectoryW(unicode_path);
	free_memory(unicode_path);

	return status;
}

/*
static Boolean file_exist (Byte* path)
{
	File      file;
	File_Data file_data;

	file = OpenFile(path, &file_data, OPEN_FILE_READ);
	CloseHandle(file);

	return !file_data.error_code;
}*/


static File create_file (Byte* path)
{
	File     file;
	Number32 bytes_writed;

	//if(file_exist(path))
	//	goto error;

	Number16* unicode_path;
	unicode_path = convert_utf8_to_unicode(path);
	file = CreateFileW(unicode_path, GENERIC_WRITE, DISABLE_ALL_FILE_OPERATION, 0, CREATE_ALWAYS, NORMAL_FILE_ATTRIBUTE, 0);
	free_memory(unicode_path);

	if(file == -1)
		return 0;

	return file;
}


static void find_files (
	Byte* search_path,
	void(*on_find)(
		Byte*    context,
		Byte*    name,
		Boolean  is_directory,
		Number32 size_low,
		Number32 size_hight
	),
	Byte* context
)
{
	Find_File_Information file_information;
	Byte*                 file_finder;
	Number16*             unicode_search_path;
	Byte*                 utf8_name;

	unicode_search_path = convert_utf8_to_unicode(search_path);
	file_finder = FindFirstFileW(unicode_search_path, &file_information);
	free_memory(unicode_search_path);

	if(file_finder)
	{
		utf8_name = convert_unicode_to_utf8(file_information.file_name);
		on_find(context, utf8_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low, file_information.file_size_high);
		free_memory(utf8_name);

		while(FindNextFileW(file_finder, &file_information))
		{
			utf8_name = convert_unicode_to_utf8(file_information.file_name);
			on_find(context, utf8_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low, file_information.file_size_high);
			free_memory(utf8_name);
		}
		FindClose(file_finder);
	}
}


static Boolean delete_file (Byte* path)
{
	Number32  status;
	Number16* unicode_path;
	
	unicode_path = convert_utf8_to_unicode(path);
	DeleteFileW(unicode_path);
	free_memory(unicode_path);

	return status;
}


static File open_file (Byte* path)
{
	File      file;
	Number16* unicode_path;

	unicode_path = convert_utf8_to_unicode(path);
	file = CreateFileW(unicode_path, GENERIC_READ | GENERIC_WRITE, /*DISABLE_ALL_FILE_OPERATION*/ENABLE_READ_FILE_OPERATION | ENABLE_WRITE_FILE_OPERATION, 0, OPEN_EXISTING, NORMAL_FILE_ATTRIBUTE, 0);//OpenFile(path, &file_data, OPEN_FILE_READ_AND_WRITE);
	free_memory(unicode_path);

	if(file == -1)
		return 0;

	return file;
}


static Number32 read_bytes_from_file(File file, Byte* bytes, Number32 size_of_bytes)
{
	Number32 bytes_readed;

	//SetFilePointer(file, position, ((Byte*)&position) + sizeof(Number32), BEGIN_FILE_POSITION);

	if(!ReadFile(file, bytes, size_of_bytes, &bytes_readed, 0)/* || bytes_readed != size_of_bytes*/)
		goto error;

	return bytes_readed;

error:
	return 0;
}


static Number32 write_bytes_in_file(File file, Byte* bytes, Number32 size_of_bytes)
{
	Number32 bytes_writed;

	//SetFilePointer(file, position, ((Byte*)&position) + sizeof(Number32), BEGIN_FILE_POSITION);

	if(!WriteFile(file, bytes, size_of_bytes, &bytes_writed, 0) || bytes_writed != size_of_bytes)
		goto error;

	return bytes_writed;

error:
	return 0;
}

/*
static Number64 get_file_size(File file)
{
	Number64 file_size;

	if(!GetFileSizeEx(file, &file_size))
	{
		Number32 bytes_returned;
		if(!DeviceIoControl(file, IOCTL_DISK_GET_LENGTH_INFO, 0, 0, &file_size, sizeof(file_size), &bytes_returned, 0))
			return 0;
	}

	return file_size;
}*/


static void offset_current_file_position(File file, Number32 position)
{
	SetFilePointer(file, position, 0, CURRENT_FILE_POSITION);
}


static void set_file_position(File file, Number32 position)
{
	SetFilePointer(file, position, 0, BEGIN_FILE_POSITION);
}


static Number get_file_position(File file)
{
	return SetFilePointer(file, 0, 0, CURRENT_FILE_POSITION);
}


static void set_file_end_position(File file)
{
	SetFilePointer(file, 0, 0, END_FILE_POSITION);
}


static void close_file (File file)
{
	CloseHandle(file);
}

/*
static void wait_for_directory_changes()
{
	File              folder;
	Number32          bytes_returned;
	File_Notification notifications[1024];

	folder = CreateFileW(
		L"./",
		FILE_LIST_DIRECTORY,
		ENABLE_READ_FILE_OPERATION | ENABLE_WRITE_FILE_OPERATION | ENABLE_DELETE_FILE_OPERATION,
		0,
		OPEN_EXISTING,
		BACKUP_SEMANTICS_FILE_ATTRIBUTE,
		0
	);

	ReadDirectoryChangesW(
		folder,
		notifications,
		sizeof(notifications),
		1,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		&bytes_returned,
		0,
		0
	);

	CloseHandle(folder);
}*/

typedef struct
{
	File              folder;
	Number32          bytes_returned;
	File_Notification notifications[1024];

	void(*on_changes)(Byte* arguments);
	Byte* on_changes_arguments;
}
Directory_Changes;

static stdcall void notify_directory_changes(Number32 error_code, Number32 bytes_transfered, Directory_Changes* changes);

static Boolean read_next_directory_changes(Directory_Changes* changes)
{
	Number status;

	changes->folder = CreateFileW(
		L"./",
		FILE_LIST_DIRECTORY,
		ENABLE_READ_FILE_OPERATION | ENABLE_WRITE_FILE_OPERATION | ENABLE_DELETE_FILE_OPERATION,
		0,
		OPEN_EXISTING,
		BACKUP_SEMANTICS_FILE_ATTRIBUTE | ASYNC_FILE_ATTRIBUTE,
		0
	);

	status = ReadDirectoryChangesW(
		changes->folder,
		changes->notifications,
		sizeof(changes->notifications),
		1,
		FILE_NOTIFY_CHANGE_LAST_WRITE,
		&changes->bytes_returned,
		changes,
		&notify_directory_changes
	);

	CloseHandle(changes->folder);

	return !!status;
}

static stdcall void notify_directory_changes(Number32 error_code, Number32 bytes_transfered, Directory_Changes* changes)
{
	read_next_directory_changes(changes);

	changes->on_changes(changes->on_changes_arguments);

	//print(
	//    "notify: error code = ", _Number, error_code,
	//    ", bytes transfered = ", _Number, bytes_transfered,
	//    ", changes = ", _Number, changes,
	//    "\n"
	//)
}

static Boolean read_directory_changes(Directory_Changes* changes, void(*on_changes)(Byte* arguments), Byte* arguments)
{
	changes->on_changes = on_changes;
	changes->on_changes_arguments = arguments;
	return read_next_directory_changes(changes);
}

#endif//__WIN32__


#include <reader.c>


void initialize_file_reader(Reader* reader, Byte* file_path)
{
	File file;

	file = open_file(file_path);

	initialize_reader(reader, file, &read_bytes_from_file);
	reader->close_source = &close_file;
}


void initialize_file_reader_from_file (Reader* reader, File file)
{
	initialize_reader(reader, file, &read_bytes_from_file);
}


void initialize_file_writer (Writer* writer, Byte* file_path)
{
	File file;

	file = create_file(file_path);

	initialize_writer(writer, file, &write_bytes_in_file);
	writer->close_source = &close_file;
}


#endif//SYSTEM_FILE_INCLUDED

/*
void main()
{	
	File file;

	Byte     data[256];
	Number32 bytes;

	file = open_file("привет.txt");
	
	if(!file)
		file = create_file("привет.txt");

	write_bytes_in_file(file, "привет", sizeof("привет") - 1);
	close_file(file);

	file = open_file("привет.txt");
	read_bytes_from_file(file, data, sizeof("привет") - 1);
	data[sizeof("привет") - 1] = '\0';
	print(
		data, " ",
		_Number, (Number32)get_file_size(file), " bytes"
	);
	close_file(file);
}*/

/*
Number16 result[20480];

void main()
{
	Number i;
	Number size;

	size = QueryDosDeviceW(0, result, 20480);

	i = 0;
	while(result[i])
	{
		for(; result[i]; ++i)
		printf("%c", result[i]);
		printf("\n");
		++i;
	}

	//Byte name[256];
	//GetVolumeInformationA("C:/", name, 256, 0, 0, 0, 0, 0);
	//printf(name);

	system("pause");
}*/
