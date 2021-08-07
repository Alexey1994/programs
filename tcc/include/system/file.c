#ifndef SYSTEM_FILE_INCLUDED
#define SYSTEM_FILE_INCLUDED


#include <types.c>


typedef Byte* File;

Boolean  create_directory(Bit16* path);
Boolean  delete_directory(Bit16* path);

Bit32    file_exist(Bit16* path);

Boolean  create_file    (Bit16* path, Byte* bytes, Number32 size_of_bytes);
void     find_files (
    Bit16* search_path,
    void(*on_find)(
        Byte*    context,
        Bit16*   name,
        Boolean  is_directory,
        Number64 size
    ),
    Byte* context
);
Boolean  delete_file    (Bit16* path);

File     open_file      (Bit16* path);
Number32 read_from_file (File file, Number64 position, Byte* bytes, Number32 size_of_bytes);
Number32 write_in_file  (File file, Number64 position, Byte* bytes, Number32 size_of_bytes);
Number64 get_file_size  (File file);
void     close_file     (File file);


#ifdef __WIN32__

#include <system/Windows/kernel32.c>


Boolean create_directory(Bit16* path)
{
    return CreateDirectoryW(path, 0);
}


Boolean delete_directory(Bit16* path)
{
    return RemoveDirectoryW(path);
}


Bit32 file_exist (Bit16* path)
{
    File      file;
    File_Data file_data;

    file = OpenFile(path, &file_data, OPEN_FILE_READ);
    CloseHandle(file);

    return !file_data.error_code;
}


Boolean create_file (Bit16* path, Byte* bytes, Number32 size_of_bytes)
{
    File     file;
    Number32 bytes_writed;

    if(file_exist(path))
        goto error;

    file = CreateFileW(path, GENERIC_WRITE, DISABLE_ALL_FILE_OPERATION, 0, CREATE_ALWAYS, NORMAL_FILE_ATTRIBUTE, 0);

    WriteFile(file, bytes, size_of_bytes, &bytes_writed, 0);

    if(bytes_writed != size_of_bytes)
        goto error2;

    CloseHandle(file);

    return 1;

error:
    return 0;

error2:
    CloseHandle(file);
    delete_file(path);
    return 0;
}


void find_files (
    Bit16* search_path,
    void(*on_find)(
        Byte*    context,
        Bit16*   name,
        Boolean  is_directory,
        Number64 size
    ),
    Byte* context
)
{
    Find_File_Information file_information;
    Byte* file_finder;

    file_finder = FindFirstFileW(search_path, &file_information);
    if(file_finder)
    {
        on_find(context, file_information.file_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low);

        while(FindNextFileW(file_finder, &file_information))
        {
            on_find(context, file_information.file_name, file_information.attributes & DIRECTORY_FILE_ATTRIBUTE, file_information.file_size_low);
        }
        FindClose(file_finder);
    }
}


Boolean delete_file (Bit16* path)
{
    return DeleteFileW(path);
}


File open_file (Bit16* path)
{
    File file;

    file = CreateFileW(path, GENERIC_READ | GENERIC_WRITE, /*DISABLE_ALL_FILE_OPERATION*/ENABLE_READ_FILE_OPERATION | ENABLE_WRITE_FILE_OPERATION, 0, OPEN_EXISTING, NORMAL_FILE_ATTRIBUTE, 0);//OpenFile(path, &file_data, OPEN_FILE_READ_AND_WRITE);

    //if(file_data.error_code)
        //goto error;
    if(file == -1)
        goto error;

    return file;

error:
    return 0;
}


Number32 read_from_file (File file, Number64 position, Byte* bytes, Number32 size_of_bytes)
{
    Number32 bytes_readed;

    SetFilePointer(file, position, ((Byte*)&position) + sizeof(Number32), BEGIN_FILE_POSITION);

    if(!ReadFile(file, bytes, size_of_bytes, &bytes_readed, 0)/* || bytes_readed != size_of_bytes*/)
        goto error;

    return bytes_readed;

error:
    return 0;
}


Number32 write_in_file (File file, Number64 position, Byte* bytes, Number32 size_of_bytes)
{
    Number32 bytes_writed;

    SetFilePointer(file, position, ((Byte*)&position) + sizeof(Number32), BEGIN_FILE_POSITION);

    if(!WriteFile(file, bytes, size_of_bytes, &bytes_writed, 0) || bytes_writed != size_of_bytes)
        goto error;

    return bytes_writed;

error:
    return 0;
}


Number64 get_file_size (File file)
{
    Number64 file_size;

    if(!GetFileSizeEx(file, &file_size))
    {
        Number32 bytes_returned;
        if(!DeviceIoControl(file, IOCTL_DISK_GET_LENGTH_INFO, 0, 0, &file_size, sizeof(file_size), &bytes_returned, 0))
            goto error;
    }

    return file_size;

error:
    printf("error");
    return 0;
}

void close_file (File file)
{
    CloseHandle(file);
}

void wait_for_directory_changes()
{
    File_Notification notifications[1024];
    File              folder;
    Number32          bytes_returned;

    folder = CreateFileW(
        L"./",
        FILE_LIST_DIRECTORY,
        ENABLE_READ_FILE_OPERATION | ENABLE_WRITE_FILE_OPERATION | ENABLE_DELETE_FILE_OPERATION,
        0,
        OPEN_EXISTING,
        BACKUP_SEMANTICS_FILE_ATTRIBUTE,
        0
    );


    ReadDirectoryChangesW(folder, notifications, sizeof(notifications), 1, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytes_returned, 0, 0);
    CloseHandle(folder);
}

#endif//__WIN32__


#endif//SYSTEM_FILE_INCLUDED

/*
void main()
{
	File file;

	Bit8  data[256];
	Bit32 bytes;

	if(!create_file(L"a.txt", 0, 0))
		log("File exist");

	file = open_file(L"a.txt");

	write_in_file(file, 0, L"Hi", 4);

	data[4] = '\0';
	read_from_file(file, 0, data, 4);
	log(data);

	log(write_N_32, (N_32)get_file_size(file), " bytes");
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