#ifndef SYSTEM_MOUSE_INCLUDED
#define SYSTEM_MOUSE_INCLUDED


#include <types.c>


Boolean pressed_left_mouse_key  ();
Boolean pressed_right_mouse_key ();
Number  get_mouse_coord_x       ();
Number  get_mouse_coord_y       ();


#ifdef __WIN32__

#include <system/Windows/user32.c>


Boolean pressed_left_mouse_key ()
{
    if(GetAsyncKeyState(1) & 0b1000000000000000)
        return 1;

    return 0;
}


Boolean pressed_right_mouse_key ()
{
    if(GetAsyncKeyState(2) & 0b1000000000000000)
        return 1;

    return 0;
}


Number get_mouse_coord_x ()
{
    Windows_Point coords;

    GetCursorPos(&coords);
    return coords.x;
}


Number get_mouse_coord_y ()
{
    Windows_Point coords;

    GetCursorPos(&coords);
    return coords.y;
}

#endif//__WIN32__


#endif//SYSTEM_MOUSE_INCLUDED

/*
void main()
{
	for(;;)
		printf("%d %d\n", get_mouse_coord_x(), get_mouse_coord_y());
}*/