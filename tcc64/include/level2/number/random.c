#ifndef RANDOM_NUMBER_INCLUDED
#define RANDOM_NUMBER_INCLUDED


#include <types.c>
#include <system/time.c>


Number previouse_random_number = 1;
Number random_number()
{
	previouse_random_number = (previouse_random_number * 60611 + get_number_of_ticks()) & 0xffff;
	return previouse_random_number;
}


#endif//RANDOM_NUMBER_INCLUDED