#ifndef GRAPHICS_LINE_H_INCLUDED
#define GRAPHICS_LINE_H_INCLUDED


#include <system/graphics/graphics.h>


typedef struct
{
    N_16 x1;
    N_16 y1;
    N_16 x2;
    N_16 y2;
}
Line;


void initialize_line (Line *line, N_16 x1, N_16 y1, N_16 x2, N_16 y2);
void draw_line       (N_32* screen, N_32 screen_width, N_32 screen_height, Line *line, N_32 color);


#define LINE(x1, y1, x2, y2)\
{\
    Line line;\
    initialize_line(&line, (x1), (y1), (x2), (y2));

#define DRAW_LINE(color)\
    draw_line(canvas, 1440, 900, &line, (color));

#define END_LINE }


#include "line.c"
#endif // GRAPHICS_LINE_H_INCLUDED
