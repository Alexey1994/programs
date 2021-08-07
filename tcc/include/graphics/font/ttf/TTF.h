#ifndef TTF_H_INCLUDED
#define TTF_H_INCLUDED


#include <system/system.h>
#include <data-structures/buffer/buffer.h>


typedef struct
{
	N_32 x;
	N_32 y;
}
Point;


typedef struct
{
	Point p1;
	Point p2;
	Point p3;
}
Curve;


typedef struct
{
	Buffer curves;
}
Vector_Glyph;


Boolean read_TTF(Buffer* glyphs, Byte* file_path);


#include "TTF.c"

#endif // TTF_H_INCLUDED
