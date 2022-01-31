#ifndef SYSTEM_TIME_INCLUDED
#define SYSTEM_TIME_INCLUDED


static Number get_number_of_ticks();


#ifdef __WIN32__

#include <system/Windows/kernel32.c>


static Number get_number_of_ticks()
{
	return GetTickCount();
}

#endif//__WIN32__


#endif//SYSTEM_TIME_INCLUDED