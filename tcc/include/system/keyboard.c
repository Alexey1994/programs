#ifndef SYSTEM_KEYBOARD_INCLUDED
#define SYSTEM_KEYBOARD_INCLUDED


#include <types.c>


Boolean pressed_key (Bit32 key_code);


#ifdef __WIN32__

#include <system/Windows/user32.c>


Boolean pressed_key(Bit32 key_code)
{
    if(GetAsyncKeyState(key_code) & 0b1000000000000000)
        return 1;

    return 0;
}

#endif//__WIN32__


#endif//SYSTEM_KEYBOARD_INCLUDED

/*
void main()
{
	while(!pressed_key('Q')){}
}*/