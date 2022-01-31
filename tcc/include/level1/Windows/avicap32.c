#ifndef SYSTEM_WINDOWS_AVICAP32_INCLUDED
#define SYSTEM_WINDOWS_AVICAP32_INCLUDED


#include <types.c>


#define WM_CAP_SET_CALLBACK_FRAME       (0x400 + 5)
#define WM_CAP_SET_CALLBACK_VIDEOSTREAM (0x400 + 6)
#define WM_CAP_GET_USER_DATA            (0x400 + 8)
#define WM_CAP_SET_USER_DATA            (0x400 + 9)

#define WM_CAP_DRIVER_CONNECT           (0x400 + 10)
#define WM_CAP_SET_PREVIEW              (0x400 + 50)
#define WM_CAP_SET_OVERLAY              (0x400 + 51)
#define WM_CAP_SET_PREVIEWRATE          (0x400 + 52)
#define WM_CAP_SET_SCALE                (0x400 + 53)

#define WM_CAP_GRAB_FRAME               (0x400 + 60)
#define WM_CAP_GRAB_FRAME_NOSTOP        (0x400 + 61)

#define WM_CAP_DLG_VIDEOFORMAT          (0x400 + 41)
#define WM_CAP_GET_VIDEOFORMAT          (0x400 + 44)
#define WM_CAP_SET_VIDEOFORMAT          (0x400 + 45)
#define WM_CAP_DLG_VIDEOCOMPRESSION     (0x400 + 46)

#define WM_CAP_FILE_SAVEDIB             (0x400 + 25)


typedef struct
{
	Byte*     data;
	Number32  buffer_length;
	Number32  bytes_used;
	Number32  time_captured;
	Number32* user_data;
	Bit32     flags;
	Bit32*    reserved[4];
}
Video_Frame;


import Bit32 capGetDriverDescriptionA(Bit16 driver_index, Bit8* name, Bit32 size_Name, Bit8* version, Bit32 size_version);
import Bit8* capCreateCaptureWindowA(
	Bit8 *window_name,
	Bit32 style,
	Number32 x,
	Number32 y,
	Number32 width,
	Number32 height,
	Bit8* handle,
	Bit32 identifer
);


#endif //SYSTEM_WINDOWS_AVICAP32_INCLUDED