#include <types.c>


#include "render/main.c"

#include "view/main.c"
#include "screen/main.c"
#include "host/main.c"


void main()
{
	Params params;
	Number time;

	Host_Init(&params);

	for(;;)
	{
		//Com_Frame();

		Host_Frame(time);
	}
}