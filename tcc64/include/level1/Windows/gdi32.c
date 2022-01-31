#ifndef SYSTEM_WINDOWS_GDI32_INCLUDED
#define SYSTEM_WINDOWS_GDI32_INCLUDED


#include <types.c>


typedef struct
{
    struct
    {
        Number32         size_of_structure;
        Integer_Number32 width;
        Integer_Number32 height;
        Number16         planes;
        Number16         bit_count;
        Number32         compression;
        Number32         size_image;
        Integer_Number32 x_pels_per_meter;
        Integer_Number32 y_pels_per_meter;
        Number32         number_of_color_indexes;
        Number32         number_of_color_indexes_important;
    }
    header;
    Number32 color[1];
}
Bitmap_Info;


import Integer_Number32     SetDIBitsToDevice(
    Byte*            device_context,
    Integer_Number32 x,
    Integer_Number32 y,
    Number32         width,
    Number32         height,
    Integer_Number32 x_image,
    Integer_Number32 y_image,
    Number32         start_scan,
    Number32         scan_lines_length,
    Byte*            image,
    Bitmap_Info*     bitmap_info,
    Number32         color_type
);


typedef struct
{
    Bit16  size;
    Bit16  nVersion;
    Bit32  dwFlags;
    Bit8   iPixelType;
    Bit8   cColorBits;
    Bit8   cRedBits;
    Bit8   cRedShift;
    Bit8   cGreenBits;
    Bit8   cGreenShift;
    Bit8   cBlueBits;
    Bit8   cBlueShift;
    Bit8   cAlphaBits;
    Bit8   cAlphaShift;
    Bit8   cAccumBits;
    Bit8   cAccumRedBits;
    Bit8   cAccumGreenBits;
    Bit8   cAccumBlueBits;
    Bit8   cAccumAlphaBits;
    Bit8   cDepthBits;
    Bit8   cStencilBits;
    Bit8   cAuxBuffers;
    Bit8   iLayerType;
    Bit8   bReserved;
    Bit32  dwLayerMask;
    Bit32  dwVisibleMask;
    Bit32  dwDamageMask;
}
Windows_Pixel_Format_Descriptor;

#define PFD_DRAW_TO_WINDOW  4
#define PFD_SUPPORT_OPENGL  32
#define PFD_DOUBLEBUFFER    1
#define PFD_TYPE_RGBA   0
#define PFD_MAIN_PLANE  0


import Number32 ChoosePixelFormat(Byte* context, Windows_Pixel_Format_Descriptor* format_descriptor);
import Bit32    SetPixelFormat(Byte* context, Number32 format, Windows_Pixel_Format_Descriptor* format_descriptor);

import Byte*    CreateSolidBrush(Number32 color);
import Byte*    CreatePatternBrush(Byte* pattern);

import Bit32    SwapBuffers(Byte* device);

import Bit32    TextOutA(Byte* device_context, Number32 x, Number32 y, Byte* text, Number32 text_length);
import Bit32    TextOutW(Byte* device_context, Number32 x, Number32 y, Bit16* text, Number32 text_length);

typedef enum
{
    FW_DONTCARE = 0,
    FW_THIN = 100,
    FW_EXTRALIGHT = 200,
    FW_LIGHT = 300,
    FW_NORMAL = 400
}
Font_Width;

typedef enum
{
    DEFAULT_QUALITY        = 0,
    DRAFT_QUALITY          = 1,
    PROOF_QUALITY          = 2,
    NONANTIALIASED_QUALITY = 3,
    ANTIALIASED_QUALITY    = 4,
    CLEARTYPE_QUALITY      = 5
}
Font_Quality;

typedef enum
{
    OUT_TT_PRECIS = 4
}
Font_Precision;

#define ANSI_CHARSET 1251

#define CLIP_DEFAULT_PRECIS 0
#define DEFAULT_PITCH 0
#define FF_DONTCARE 0

import Byte* CreateFontW(
    Number32 height,
    Number32 width,
    Number32 escapement,
    Number32 orientation,
    Number32 weight,
    Number32 italic,
    Number32 underline,
    Number32 strike_out,
    Number32 charset,
    Number32 out_precision,
    Number32 clip_precision,
    Number32 quality,
    Number32 pitch_and_family,
    Number16* face_name
);

import Number32 SelectObject(Byte* device, Byte* object);

typedef enum
{
    TRANSPARENT = 1,
    OPAQUE      = 2
}
Bk_Mode;

import Number32 SetBkMode(Byte* device, Number32 mode);


#endif //SYSTEM_WINDOWS_GDI32_INCLUDED