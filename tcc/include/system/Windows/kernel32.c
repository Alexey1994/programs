#ifndef SYSTEM_WINDOWS_KERNEL32_INCLUDED
#define SYSTEM_WINDOWS_KERNEL32_INCLUDED


#include <types.c>

/*
//https://github.com/tpn/winsdk-10/blob/master/Include/10.0.14393.0/shared/ntdddisk.h
//https://github.com/tpn/winsdk-10/blob/master/Include/10.0.16299.0/shared/devioctl.h

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define FILE_DEVICE_DISK                0x00000007
#define IOCTL_DISK_BASE                 FILE_DEVICE_DISK

#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe*/

typedef enum
{
	IOCTL_DISK_GET_DRIVE_GEOMETRY = 0x70000, //(0x00000007 << 16) | (0 << 14) | (0 << 2) | 0 //CTL_CODE(IOCTL_DISK_BASE, 0x0000, METHOD_BUFFERED, FILE_ANY_ACCESS)
	IOCTL_DISK_GET_PARTITION_INFO = 0x74004, //(0x00000007 << 16) | (1 << 14) | (1 << 2) | 0 //CTL_CODE(IOCTL_DISK_BASE, 0x0001, METHOD_BUFFERED, FILE_READ_ACCESS)
	IOCTL_DISK_GET_LENGTH_INFO    = 0x7405C  //(0x00000007 << 16) | (1 << 14) | (23 << 2) | 0// CTL_CODE(IOCTL_DISK_BASE, 0x0017, METHOD_BUFFERED, FILE_READ_ACCESS)
}
IO_Control_Code;


import Boolean DeviceIoControl(
	Byte*     handle,
	Number32  control_code,
	Byte*     in_buffer,
	Number32  in_buffer_size,
	Byte*     out_buffer,
	Number32  out_buffer_size,
	Number32* bytes_returned,
	Byte*     overlapped
);


//=============================== File ===============================


typedef struct
{
	Number32 size;
	Byte*    security_descriptor;
	Boolean  inherit_handle;
}
Security_Attributes;

Boolean CreateDirectoryW(Bit16* path, Security_Attributes* attributes);
Boolean RemoveDirectoryW(Bit16* path);


typedef Bit8* File;

//https://docs.microsoft.com/ru-ru/windows/desktop/api/winbase/ns-winbase-_ofstruct
typedef struct
{
	Bit8  size_struct;
	Bit8  file_on_hard_disk;
	Bit16 error_code;
	Bit16 reserved1;
	Bit16 reserved2;
	Bit8  path_name[256];
}
File_Data;

//https://docs.microsoft.com/en-us/windows/desktop/api/winbase/nf-winbase-openfile
typedef enum
{
	OPEN_FILE_READ           = 0,
	OPEN_FILE_WRITE          = 1,
	OPEN_FILE_READ_AND_WRITE = 2
}
Open_File_Option;

import File OpenFile    (Bit8* path, File_Data* file_data, Open_File_Option options);
import Bit8 CloseHandle (File file);

typedef enum
{
	GENERIC_READ        = 0x80000000,
	GENERIC_WRITE       = 0x40000000,
	FILE_LIST_DIRECTORY = 0x00000001
}
Create_File_Mode;

typedef enum
{
	DISABLE_ALL_FILE_OPERATION   = 0,
	ENABLE_READ_FILE_OPERATION   = 1,
	ENABLE_WRITE_FILE_OPERATION  = 2,
	ENABLE_DELETE_FILE_OPERATION = 4
}
Enabled_File_Operation_For_Other_Processes;

typedef enum
{
	CREATE_NEW        = 1,
	CREATE_ALWAYS     = 2,
	OPEN_EXISTING     = 3,
	OPEN_ALWAYS       = 4,
	TRUNCATE_EXISTING = 5,
}
Create_File_Action;

typedef enum
{
	READONLY_FILE_ATTRIBUTE         = 1,
	HIDDEN_FILE_ATTRIBUTE           = 2,
	SYSTEM_FILE_ATTRIBUTE           = 4,
	DIRECTORY_FILE_ATTRIBUTE        = 16,
	ARCHIVE_FILE_ATTRIBUTE          = 32,
	DEVICE_FILE_ATTRIBUTE           = 64,
	NORMAL_FILE_ATTRIBUTE           = 128,
	TEMPORARY_FILE_ATTRIBUTE        = 256,
	SPARSE_FILE_FILE_ATTRIBUTE      = 512,
	REPARSE_POINT_FILE_ATTRIBUTE    = 1024,
	COMPRESSED_FILE_ATTRIBUTE       = 2048,
	OFFLINE_FILE_ATTRIBUTE          = 4096,
	NOT_INDEXED_FILE_ATTRIBUTE      = 8192,
	ENCRYPTED_FILE_ATTRIBUTE        = 16384,
	NO_BUFFERING_FILE_ATTRIBUTE     = 0x20000000,
	BACKUP_SEMANTICS_FILE_ATTRIBUTE = 0x02000000
}
File_Attribute;


import File CreateFileA(
	Bit8*                                       name,
	Create_File_Mode                            mode,
	Enabled_File_Operation_For_Other_Processes  flags2,
	void*                                       security_attributes, // 0 если не наследуется дочерними процессами
	Create_File_Action                          create_action,
	File_Attribute                              attributes,
	File                                        file
);

import File CreateFileW(
	Bit16*                                      name,
	Create_File_Mode                            mode,
	Enabled_File_Operation_For_Other_Processes  flags2,
	void*                                       security_attributes, // 0 если не наследуется дочерними процессами
	Create_File_Action                          create_action,
	File_Attribute                              attributes,
	File                                        file
);

typedef enum
{
	MOVE_FILE_COPY_ALLOWED = 0x2
}
Move_File_Option;

import Number MoveFileExW(Bit16* old_path, Bit16* new_path, Number32 options);

import Bit8 DeleteFileA(Bit8* path);
import Bit8 DeleteFileW(Bit16* path);

import Bit8 ReadFile(
	File   file,
	Bit8*  buffer,
	Bit32  buffer_length,
	Bit32* bytes_readed,
	Bit8*  overlapped
);

import Bit8 WriteFile (
	File   file,
	Bit8*  data,
	Bit32  data_length,
	Bit32* bytes_writed,
	Bit8*  overlapped
);

typedef enum
{
	BEGIN_FILE_POSITION   = 0,
	CURRENT_FILE_POSITION = 1,
	END_FILE_POSITION     = 2
}
File_Pointer_Position;

//https://docs.microsoft.com/ru-ru/windows/desktop/api/fileapi/nf-fileapi-setfilepointer
import Bit32 SetFilePointer (
	File                   file,
	Bit32                  distance,
	Bit32*                 distance_high,
	File_Pointer_Position  method
);

/*
typedef struct
{
	Bit32 dwLowDateTime;
	Bit32 dwHighDateTime;
}
File_Time;
*/

//https://docs.microsoft.com/ru-ru/windows/desktop/api/fileapi/ns-fileapi-_by_handle_file_information
typedef struct
{
	Bit32  attributes;
	Bit64  creation_time;
	Bit64  last_access_time;
	Bit64  last_write_time;
	Bit32  volume_serial_number;
	Bit32  size_high;
	Bit32  size_low;
	Bit32  number_of_links;
	Bit32  file_index_high;
	Bit32  file_index_low;
}
File_Information;

import Bit32 GetFileInformationByHandle(File file, File_Information* information);

typedef enum
{
	DRIVE_UNKNOWN     = 0,
	DRIVE_NO_ROOT_DIR = 1,
	DRIVE_REMOVABLE   = 2,
	DRIVE_FIXED       = 3,
	DRIVE_REMOTE      = 4,
	DRIVE_CDROM       = 5,
	DRIVE_RAMDISK     = 6
}
Drive_Type;

import Drive_Type GetDriveTypeA(Byte* path);
import Number32 GetLogicalDrives();
import Bit32 GetVolumeInformationA(
	Byte*     root,
	Byte*     name,
	Number32  name_size,
	Number32* serial_number,
	Number32* maximum_file_name_size,
	Number32* file_system_flags,
	Byte*     file_system_name,
	Number32  file_system_name_size
);
import Bit32 GetVolumeInformationW(
	Number16* root,
	Number16* name,
	Number32  name_size,
	Number32* serial_number,
	Number32* maximum_file_name_size,
	Number32* file_system_flags,
	Number16* file_system_name,
	Number32  file_system_name_size
);


import Number32 QueryDosDeviceW(Byte* device_path, Number16* device_name, Number32 size_of_device_name);


typedef struct
{
	Number32 attributes;
	Bit64    creation_time;
	Bit64    last_access_time;
	Bit64    last_write_time;
	Number32 file_size_high;
	Number32 file_size_low;
	Number32 reserved1;
	Number32 reserved2;
	Byte     file_name[260];
	Byte     alternate_file_name[14];
	Number32 file_type;
	Number32 creator_type;
	Number16 file_attributes;
}
Find_File_InformationA;

import Byte* FindFirstFileA(Bit8* path, Find_File_InformationA* information);

typedef struct
{
	Number32 attributes;
	Bit64    creation_time;
	Bit64    last_access_time;
	Bit64    last_write_time;
	Number32 file_size_high;
	Number32 file_size_low;
	Number32 reserved1;
	Number32 reserved2;
	Bit16    file_name[260];
	Bit16    alternate_file_name[14];
	//Number32 file_type;
	//Number32 creator_type;
	//Number16 file_attributes;
}
Find_File_Information;

import Byte*   FindFirstFileW(Bit8* path, Find_File_Information* information);
import Boolean FindNextFileW(Byte* find_state, Find_File_Information* information);
import Boolean FindClose(Byte* find_state);

import Boolean GetFileSizeEx(File file, Number64* size);


typedef enum
{
	FILE_NOTIFY_CHANGE_FILE_NAME   = 0x00000001,
	FILE_NOTIFY_CHANGE_DIR_NAME    = 0x00000002,
	FILE_NOTIFY_CHANGE_ATTRIBUTES  = 0x00000004,
	FILE_NOTIFY_CHANGE_SIZE        = 0x00000008,
	FILE_NOTIFY_CHANGE_LAST_WRITE  = 0x00000010,
	FILE_NOTIFY_CHANGE_LAST_ACCESS = 0x00000020,
	FILE_NOTIFY_CHANGE_CREATION    = 0x00000040,
	FILE_NOTIFY_CHANGE_SECURITY    = 0x00000100
}
File_Notify_Filter;

typedef enum
{
	FILE_ADDED            = 0x00000001,
	FILE_REMOVED          = 0x00000002,
	FILE_MODIFIED         = 0x00000003,
	FILE_RENAMED_OLD_NAME = 0x00000004,
	FILE_RENAMED_NEW_NAME = 0x00000005
}
File_Notification_Type;

typedef struct
{
	Number32               size;
	File_Notification_Type type;
	Number32               file_name_size;
	Number16               file_name[];
}
File_Notification;

import Boolean ReadDirectoryChangesW(
	Byte*              directory,
	File_Notification* notifications,
	Number32           size_of_notifications,
	Boolean            watch_subtree,
	Number32           notify_filter,
	Number32*          bytes_returned,
	Byte*              overlapped,
	void             (*notify)(Number32 error_code, Number32 bytes_transfered, Byte* overlapped)
);

//=============================== Memory =============================

//https://docs.microsoft.com/en-us/windows/desktop/api/memoryapi/nf-memoryapi-virtualalloc
typedef enum
{
	COMMIT_MEMORY = 0x00001000,
	RESERVE_MEMORY = 0x00002000,
	RESET_MEMORY = 0x00080000
	//...
}
Memory_Allocation_Type;

//https://docs.microsoft.com/ru-ru/windows/desktop/Memory/memory-protection-constants
typedef enum
{
	EXECUTE_MEMORY_ACCESS                = 0x10,
	EXECUTE_AND_READ_MEMORY_ACCESS       = 0x20,
	EXECUTE_READ_AND_WRITE_MEMORY_ACCESS = 0x40,
	NO_MEMORY_ACCESS                     = 1,
	GUARD_MEMORY_ACCESS                  = 0x100
	//...
}
Memory_Protection_Option;

import Bit8* VirtualAlloc (Bit8* start_address, Bit32 size, Memory_Allocation_Type type, Memory_Protection_Option protection);

//https://docs.microsoft.com/en-us/windows/desktop/api/memoryapi/nf-memoryapi-virtualfree
typedef enum
{
	RELEASE_MEMORY = 0x00008000
	//...
}
Memory_Free_Type;

import void VirtualFree (Bit8* address, Bit32 size, Memory_Free_Type type);


typedef enum
{
	HEAP_GENERATE_EXCEPTIONS   = 0x00000004,
	HEAP_NO_SERIALIZE          = 0x00000001,
	HEAP_REALLOC_IN_PLACE_ONLY = 0x00000010,
	HEAP_ZERO_MEMORY           = 0x00000008
}
Heap_Flags;

import Bit8* GetProcessHeap ();
import Bit8* HeapAlloc      (Byte* heap, Bit32 flags, Number32 bytes_length);
import void  HeapFree       (Bit8* heap, Bit32 flags, Bit8* bytes);
import Bit8* HeapReAlloc    (Byte* heap, Bit32 flags, Bit8* bytes, Number32 bytes_length);


//=============================== Exceptions =========================

typedef struct
{
	Number32          code;
	Number32          flags;
	struct Exception* next_exception;
	Byte*             address;
	Number32          number_parameters;
	Number32          information[15];
}
Exception;

typedef struct
{
	Number32   ControlWord;
	Number32   StatusWord;
	Number32   TagWord;
	Number32   ErrorOffset;
	Number32   ErrorSelector;
	Number32   DataOffset;
	Number32   DataSelector;
	Byte       RegisterArea[80];
	Number32   Cr0NpxState;
}
Floating_Context;

typedef struct
{
	Number32   ContextFlags;
	Number32   Dr0;
	Number32   Dr1;
	Number32   Dr2;
	Number32   Dr3;
	Number32   Dr6;
	Number32   Dr7;
	Floating_Context FloatSave;
	Number32   SegGs;
	Number32   SegFs;
	Number32   SegEs;
	Number32   SegDs;
	Number32   Edi;
	Number32   Esi;
	Number32   Ebx;
	Number32   Edx;
	Number32   Ecx;
	Number32   Eax;
	Number32   Ebp;
	Number32   Eip;
	Number32   SegCs;
	Number32   EFlags;
	Number32   Esp;
	Number32   SegSs;
	Byte       ExtendedRegisters[512];
}
Context;

typedef struct
{
	Exception* exception;
	Context*   context;
}
Exceptions_List;

//https://docs.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-addvectoredexceptionhandler
import Number32 AddVectoredExceptionHandler(Number32 priority, void(*handler)(Exceptions_List* exceptions));


//=============================== Process ============================

typedef struct
{
	Number32  size;
	Number16* reserved;
	Number16* desktop;
	Number16* title;
	Number32  x;
	Number32  y;
	Number32  width;
	Number32  height;
	Number32  number_of_x_chars;
	Number32  number_of_y_chars;
	Number32  fill_attribute;
	Number32  flags;
	Number16  show_window;
	Number16  reserved2;
	Byte*     reserved3;
	Byte*     std_in;
	Byte*     std_out;
	Byte*     std_error;
}
Sturtup_Info;

typedef struct
{
	Byte*    process;
	Byte*    thread;
	Number32 process_id;
	Number32 thread_id;
}
Process_Information;

import Boolean CreateProcessW(
	Number16*            module_name,
	Number16*            command,
	Number32             process_security_attributes,
	Number32             thread_security_attributes,
	Boolean              inherit_handles,
	Number32             creation_flags,
	void*                environment,
	Number16*            current_directory,
	Sturtup_Info*        startup_info,
	Process_Information* process_information
);


//=============================== Thread =============================

typedef struct
{
	Number32  length;
	Bit8*     security_descriptor;
	Bit8      is_inherit_handle;
}
Windows_Sequrity_Attributes;


import void  Sleep        (Bit32 milliseconds);
import Bit32 CreateThread (
	Windows_Sequrity_Attributes* security_attributes,
	Number32                     stack_size,
	void                       (*thread_function)(Bit8* arguments),
	Bit8*                        attributes,
	Bit32                        creation_flags,
	Bit32*                       thread_id
);


//=============================== Console ============================


typedef enum
{
	STD_INPUT_HANDLE   = -10,
	STD_OUTPUT_HANDLE  = -11,
	STD_ERROR_HANDLE   = -12
}
Std_Handle;

import Byte* GetStdHandle(Std_Handle handle);

typedef enum
{
	FOREGROUND_BLUE            = 0x0001,
	FOREGROUND_GREEN           = 0x0002,
	FOREGROUND_RED             = 0x0004,
	FOREGROUND_INTENSITY       = 0x0008,
	BACKGROUND_BLUE            = 0x0010,
	BACKGROUND_GREEN           = 0x0020,
	BACKGROUND_RED             = 0x0040,
	BACKGROUND_INTENSITY       = 0x0080,
	COMMON_LVB_LEADING_BYTE    = 0x0100,
	COMMON_LVB_TRAILING_BYTE   = 0x0200,
	COMMON_LVB_GRID_HORIZONTAL = 0x0400,
	COMMON_LVB_GRID_LVERTICAL  = 0x0800,
	COMMON_LVB_GRID_RVERTICAL  = 0x1000,
	COMMON_LVB_REVERSE_VIDEO   = 0x4000,
	COMMON_LVB_UNDERSCORE      = 0x8000
}
Console_Attribute;

import Bit32 SetConsoleTextAttribute(Byte* console, Number16 attribute);

import Bit32 WriteConsoleA(
	Byte*     console_handle,
	Byte*     charachters,
	Number32  charachters_length,
	Number32* charachters_written,
	Byte*     reserved
);


//=============================== Library ============================


import Byte* LoadLibraryA   (Byte* file_name);
import Byte* LoadLibraryW   (Bit16* file_name);
import Byte* GetProcAddress (Byte* library, Byte* procedure_name);


//=============================================

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 256
#define FORMAT_MESSAGE_IGNORE_INSERTS 512
#define FORMAT_MESSAGE_FROM_STRING 1024
#define FORMAT_MESSAGE_FROM_HMODULE 2048
#define FORMAT_MESSAGE_FROM_SYSTEM 4096
#define FORMAT_MESSAGE_ARGUMENT_ARRAY 8192
#define FORMAT_MESSAGE_MAX_WIDTH_MASK 255

import Number32 FormatMessageA(
	Number32 flags,
	Byte*    source,
	Number32 message_id,
	Number32 language_id,
	Byte*    buffer,
	Number32 buffer_size,
	Byte*    arguments
);


typedef enum
{
	CP_ACP = 0
}
Code_Page;

import Number32 WideCharToMultiByte(Number32 code_page, Number32 flags, Number16* wide_chars, Number32 number_of_wide_chars, Byte* chars, Number32 number_of_chars, Byte* default_char, Boolean* used_default_char);


import Number32 GetTickCount();


#endif //SYSTEM_WINDOWS_KERNEL32_INCLUDED