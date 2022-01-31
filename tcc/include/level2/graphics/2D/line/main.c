#include "line.h"
#include <system/mouse/mouse.h>


N_32 main()
{
	Graphics graphics;

	initialize_graphics(&graphics, 1440, 900);

	N_32* canvas = malloc(1440 * 900 * 4);

	for(;;)
	{
		LINE(500, 500, get_mouse_coord_x(), get_mouse_coord_y())
			DRAW_LINE(255 * 256 + (255 << 24))
		END_LINE

		glClear(GL_COLOR_BUFFER_BIT);
        glRasterPos2f(-1, -1);
        glDrawPixels(1440, 900, GL_RGBA, GL_UNSIGNED_BYTE, canvas);

		draw_graphics(&graphics);
	}

    return 0;
}