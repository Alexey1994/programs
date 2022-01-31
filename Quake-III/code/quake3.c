#include <types.c>


#include "client/main.c"
#include "common/main.c"


void main()
{
	CL_Init(0);

	for(;;)
	{
		Com_Frame();
	}
}